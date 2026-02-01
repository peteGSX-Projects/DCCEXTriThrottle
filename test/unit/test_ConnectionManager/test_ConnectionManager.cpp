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
#include "test/mocks/DCCEXTestHelpers.h"
#include "test/mocks/Stream.h"
#include <DCCEXProtocol.h>
#include <gtest/gtest.h>

using namespace testing;

/**
 * @brief Test harness for all ConnectionManager tests
 */
class ConnectionManagerTests : public Test {
protected:
  DCCEXProtocolDelegate *mockDelegate;
  DCCEXProtocol *mockClient;
  EventManager *eventManager;
  ConnectionManager *connectionManager;
  Stream mockStream;

  void SetUp() override {
    millis();
    mockDelegate = new DCCEXProtocolDelegate;
    mockClient = new DCCEXProtocol;
    mockClient->setDelegate(mockDelegate);
    mockClient->connect(&mockStream);
    eventManager = new EventManager(nullptr);
    connectionManager = new ConnectionManager(mockClient, eventManager, nullptr);
  }

  void TearDown() override {
    delete connectionManager;
    delete eventManager;
    delete mockClient;
    delete mockDelegate;
  }
};

/**
 * @brief Test that begin() attempts to start the connection
 */
TEST_F(ConnectionManagerTests, TestBeginInitiatesConnection) {
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
  // Start with begin()
  connectionManager->begin();

  // Should start in Connecting state
  ASSERT_EQ(connectionManager->getState(), ConnectionState::Connecting);

  // Set success handshake
  DCCEXTestHelpers::injectSuccessHandshakeFullLists(mockStream);

  // Should take at least 8 updates() to complete getting lists and complete
  for (int i = 0; i < 9; i++) {
    connectionManager->update();
    if (connectionManager->getState() == ConnectionState::Connected)
      break;
  }

  // Validation
  EXPECT_EQ(connectionManager->getState(), ConnectionState::Connected);
}

/**
 * @brief Test that connection fails after retries are exceeded
 */
TEST_F(ConnectionManagerTests, TestConnectionRetryTransition) {
  // Start with begin()
  connectionManager->begin();

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
}
