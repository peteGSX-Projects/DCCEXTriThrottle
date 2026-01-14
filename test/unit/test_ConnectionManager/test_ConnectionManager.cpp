/*
 *  © 2026 Peter Cole
 *
 *  This is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  It is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this code.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "Arduino.h"
#include "ConnectionManager.h"
#include "EventManager.h"
#include "test/mocks/MockDCCEXProtocol.h"
#include <gtest/gtest.h>

using namespace testing;

/**
 * @brief Test harness for all ConnectionManager tests
 */
class ConnectionManagerTests : public Test {
protected:
  MockDCCEXProtocol *mockClient;
  EventManager *eventManager;
  ConnectionManager *connectionManager;

  void SetUp() override {
    millis();
    mockClient = new MockDCCEXProtocol;
    eventManager = new EventManager(nullptr);
    connectionManager = new ConnectionManager(mockClient, eventManager, nullptr);
  }

  void TearDown() override {
    delete connectionManager;
    delete eventManager;
    delete mockClient;
  }
};

/**
 * @brief Test that begin() attempts to start the connection
 */
TEST_F(ConnectionManagerTests, TestBeginInitiatesConnection) {
  // Expect getLists() is called for all object types
  EXPECT_CALL(*mockClient, getLists(true, true, true, true)).Times(1);

  // Initial connection state should be None as it hasn't done anything yet
  EXPECT_EQ(connectionManager->getState(), ConnectionState::None);

  // Call begin()
  connectionManager->begin();

  // State should now be Connecting
  EXPECT_EQ(connectionManager->getState(), ConnectionState::Connecting);
}

/**
 * @brief Test that receiving all lists transitions to Connected state
 */
TEST_F(ConnectionManagerTests, TestConnectionSuccessTransition) {
  // We know receivedLists() will be called a lot, suppress this
  EXPECT_CALL(*mockClient, receivedLists()).WillRepeatedly(Invoke([this]() {
    return this->mockClient->DCCEXProtocol::receivedLists();
  }));

  // We also should expect getLists() to be called
  EXPECT_CALL(*mockClient, getLists(testing::_, testing::_, testing::_, testing::_)).Times(1);

  // Start with begin()
  connectionManager->begin();

  // Ensure lists aren't received yet and state is Connecting
  mockClient->setReceivedRoster(false);
  connectionManager->update();
  EXPECT_EQ(connectionManager->getState(), ConnectionState::Connecting);

  // Now set all lists as received
  mockClient->setReceivedRoster(true);
  mockClient->setReceivedTurnoutList(true);
  mockClient->setReceivedRouteList(true);
  mockClient->setReceivedTurntableList(true);

  // Call update again which should update state to Connected
  connectionManager->update();
  EXPECT_EQ(connectionManager->getState(), ConnectionState::Connected);

  // Clean up expectations
  testing::Mock::VerifyAndClearExpectations(mockClient);
  testing::Mock::VerifyAndClearExpectations(connectionManager);
}

/**
 * @brief Test that connection fails after retries are exceeded
 */
TEST_F(ConnectionManagerTests, TestConnectionRetryTransition) {
  // We know receivedLists() will be called a lot, suppress this
  EXPECT_CALL(*mockClient, receivedLists()).WillRepeatedly(Invoke([this]() {
    return this->mockClient->DCCEXProtocol::receivedLists();
  }));

  // We should have a total of CONNECT_RETRIES plus the begin() call to getLists()
  EXPECT_CALL(*mockClient, getLists(true, true, true, true))
      .Times(CONNECT_RETRIES + 1)
      .WillRepeatedly(
          Invoke([this](bool a, bool b, bool c, bool d) { this->mockClient->DCCEXProtocol::getLists(a, b, c, d); }));

  // Start with begin()
  connectionManager->begin();

  // Immediate update, should be connecting with no retry
  connectionManager->update();
  EXPECT_EQ(connectionManager->getState(), ConnectionState::Connecting);

  // Call update while advancing time according to retry delay and retries
  for (int i = 0; i < CONNECT_RETRIES; i++) {
    advanceMillis(CONNECT_RETRY_DELAY + 1);
    connectionManager->update();
    // State should continue to be Connecting
    EXPECT_EQ(connectionManager->getState(), ConnectionState::Connecting);
  }

  // Advance another time and connection should fail
  advanceMillis(CONNECT_RETRY_DELAY + 1);
  connectionManager->update();
  EXPECT_EQ(connectionManager->getState(), ConnectionState::Failed);

  // Clean up expectations
  testing::Mock::VerifyAndClearExpectations(mockClient);
  testing::Mock::VerifyAndClearExpectations(connectionManager);
}
