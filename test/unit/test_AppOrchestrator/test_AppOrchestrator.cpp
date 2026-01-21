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

#include "AppOrchestrator.h"
#include "EventManager.h"
#include "Throttle.h"
#include "Version.h"
#include "test/mocks/DCCEXProtocol.h"
#include "test/mocks/MockButton.h"
#include "test/mocks/MockConnectionManager.h"
#include "test/mocks/MockDisplay.h"
#include "test/mocks/MockKeypad.h"
#include "test/mocks/MockRotaryEncoder.h"
#include "test/mocks/Stream.h"
#include <gtest/gtest.h>

using namespace testing;

/// @brief Test harness for AppOrchestrator
class AppOrchestratorTests : public Test {
protected:
  AppOrchestrator *appOrchestrator;
  MockDisplay *mockDisplay;
  MockKeypad *mockKeypad;
  Logger *logger;
  MockButton *button1;
  MockButton *button2;
  MockButton *button3;
  MockRotaryEncoder *encoder1;
  MockRotaryEncoder *encoder2;
  MockRotaryEncoder *encoder3;
  MockConnectionManager *connectionManager;
  Throttle *throttles[NUM_THROTTLES];
  EventManager *eventManager;
  MenuManager *menuManager;
  Stream csConnection;
  DCCEXProtocol *csClient;

  void SetUp() override {
    mockDisplay = new MockDisplay;
    mockKeypad = new MockKeypad;
    logger = new Logger;
    button1 = new MockButton;
    button2 = new MockButton;
    button3 = new MockButton;
    encoder1 = new MockRotaryEncoder;
    encoder2 = new MockRotaryEncoder;
    encoder3 = new MockRotaryEncoder;
    csClient = new DCCEXProtocol;
    csClient->connect(&csConnection);
    throttles[0] = new Throttle(0, button1, encoder1, csClient, logger, 1, 2, 5);
    throttles[1] = new Throttle(1, button2, encoder2, csClient, logger, 1, 2, 5);
    throttles[2] = new Throttle(2, button3, encoder3, csClient, logger, 1, 2, 5);
    connectionManager = new MockConnectionManager;
    eventManager = new EventManager(nullptr);
    menuManager = new MenuManager(eventManager, logger);
    appOrchestrator = new AppOrchestrator(mockDisplay, mockKeypad, logger, NUM_THROTTLES, throttles, connectionManager,
                                          eventManager, menuManager, csClient);
  }

  void TearDown() override {
    delete appOrchestrator;
    delete menuManager;
    delete eventManager;
    delete connectionManager;

    for (int i = 0; i < 3; i++) {
      delete throttles[i];
    }

    delete csClient;
    delete encoder3;
    delete encoder2;
    delete encoder1;
    delete button3;
    delete button2;
    delete button1;
    delete logger;
    delete mockKeypad;
    delete mockDisplay;
  }
};

/**
 * @brief Ensure AppOrchestrator starts in Startup state
 */
TEST_F(AppOrchestratorTests, ValidateStartupState) {
  EXPECT_EQ(appOrchestrator->getCurrentAppState(), AppState::Startup);
}

/**
 * @brief Ensure AppOrchestrator::begin() starts connection manager
 */
TEST_F(AppOrchestratorTests, TestBeginStartsConnectionManager) {
  // Set expectation
  EXPECT_CALL(*connectionManager, begin()).Times(1);
  appOrchestrator->begin();
}

/**
 * @brief Test AppState changes to Throttle automatically when connected
 */
TEST_F(AppOrchestratorTests, TestTransitionOnConnectionSuccess) {
  // update() should be called twice during this test
  EXPECT_CALL(*connectionManager, update()).Times(2);
  // getState() should return Connecting and then Connected
  EXPECT_CALL(*connectionManager, getState())
      .WillOnce(Return(ConnectionState::Connecting))
      .WillOnce(Return(ConnectionState::Connected));

  // We expect the Throttle screen to be displayed once
  EXPECT_CALL(*mockDisplay, displayThrottleScreen()).Times(1);

  // First update() should be Startup
  appOrchestrator->update();
  EXPECT_EQ(appOrchestrator->getCurrentAppState(), AppState::Startup);

  // Second update should be connected, and therefore move to Throttle
  appOrchestrator->update();
  EXPECT_EQ(appOrchestrator->getCurrentAppState(), AppState::Throttle);
}

/**
 * @brief Test AppState changes to ConnectionError automatically when connection fails
 */
TEST_F(AppOrchestratorTests, TestTransitionOnConnectionFail) {
  // update() should be called twice during this test
  EXPECT_CALL(*connectionManager, update()).Times(1);
  // getState() should return Failed
  EXPECT_CALL(*connectionManager, getState()).WillOnce(Return(ConnectionState::Failed));

  // We expect the Throttle screen to be displayed once
  EXPECT_CALL(*mockDisplay, displayConnectionErrorScreen()).Times(1);

  // First update() should be Startup
  appOrchestrator->update();
  EXPECT_EQ(appOrchestrator->getCurrentAppState(), AppState::ConnectionError);
}

/**
 * @brief Test user input causes connection retry
 */
TEST_F(AppOrchestratorTests, TestConnectionRetry) {
  // Force connection error
  EXPECT_CALL(*connectionManager, getState()).WillRepeatedly(Return(ConnectionState::Failed));
  appOrchestrator->update();
  ASSERT_EQ(appOrchestrator->getCurrentAppState(), AppState::ConnectionError);

  // When user presses any key (except * which is demo mode), should call connectionManager->begin()
  EXPECT_CALL(*connectionManager, begin()).Times(1);

  // Key press
  mockKeypad->setInputEvent({'1', UserInputInterface::UserInputAction::Pressed});

  // update()
  appOrchestrator->update();

  // Should be back in AppState::Startup now
  EXPECT_EQ(appOrchestrator->getCurrentAppState(), AppState::Startup);
}

/**
 * @brief Ensure AppOrchestrator event subscriptions work
 */
TEST_F(AppOrchestratorTests, TestEventSubscription) {
  // Call begin() to setup subscriptions
  appOrchestrator->begin();

  // Setup expectations
  EXPECT_CALL(*mockDisplay, updateProgressScreen()).Times(1);

  // Publish event
  eventManager->publish(EventType::ConnectionRetry, EventData());
}

/**
 * @brief Test successful transition from Throttle to Menu
 */
TEST_F(AppOrchestratorTests, TestThrottleToMenu) {
  // Connection success should have Throttle state
  EXPECT_CALL(*connectionManager, getState()).WillOnce(Return(ConnectionState::Connected));
  appOrchestrator->update();

  ASSERT_EQ(appOrchestrator->getCurrentAppState(), AppState::Throttle);

  // Now press '*' should move to Menu
  mockKeypad->setInputEvent({'*', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();
  EXPECT_EQ(appOrchestrator->getCurrentAppState(), AppState::Menu);
}
