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
#include "Throttle.h"
#include "Version.h"
#include "test/mocks/MockButton.h"
#include "test/mocks/MockDisplay.h"
#include "test/mocks/MockKeypad.h"
#include "test/mocks/MockRotaryEncoder.h"
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
  static const int NUM_THROTTLES = 3;
  Throttle *throttles[NUM_THROTTLES];

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
    throttles[0] = new Throttle(0, button1, encoder1, 1, 2, 5);
    throttles[1] = new Throttle(1, button2, encoder2, 1, 2, 5);
    throttles[2] = new Throttle(2, button3, encoder3, 1, 2, 5);
    appOrchestrator = new AppOrchestrator(mockDisplay, mockKeypad, logger, NUM_THROTTLES, throttles);
  }

  void TearDown() override {
    delete mockDisplay;
    delete mockKeypad;
    delete logger;

    for (int i = 0; i < 3; i++) {
      delete throttles[i];
    }

    delete button1;
    delete button2;
    delete button3;
    delete encoder1;
    delete encoder2;
    delete encoder3;
    delete appOrchestrator;
  }
};

/**
 * @brief Ensure AppOrchestrator starts in Startup state
 */
TEST_F(AppOrchestratorTests, ValidateStartupState) {
  EXPECT_EQ(appOrchestrator->getCurrentAppState(), AppState::Startup);
}

/**
 * @brief Ensure AppOrchestrator goes from Startup to Throttle when any key event occurs, should also cause display
 * redraw
 */
TEST_F(AppOrchestratorTests, ValidateStartuptoThrottleState) {
  const char *expectedText = "DCC-EX Tri-Throttle";
  const char *expectedVersion = VERSION;

  // Validate starts in Startup
  EXPECT_EQ(appOrchestrator->getCurrentAppState(), AppState::Startup);

  // Startup and throttle display should be called once and once only
  EXPECT_CALL(*mockDisplay, displayStartupScreen(StrEq(expectedText), StrEq(expectedVersion))).Times(1);
  EXPECT_CALL(*mockDisplay, displayThrottleScreen()).Times(1);

  // Needs redraw should start true
  EXPECT_TRUE(mockDisplay->needsRedraw());

  // Single update with no key presses should remain in Startup, and display startup should be called
  appOrchestrator->update();
  EXPECT_EQ(appOrchestrator->getCurrentAppState(), AppState::Startup);

  // Display should not need redraw after initial update
  EXPECT_FALSE(mockDisplay->needsRedraw());

  // Key press should change to Throttle
  mockKeypad->setInputEvent({'1', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();
  EXPECT_EQ(appOrchestrator->getCurrentAppState(), AppState::Throttle);

  // Subsequent update with no user input should maintain state and not call startup display again
  appOrchestrator->update();
  EXPECT_EQ(appOrchestrator->getCurrentAppState(), AppState::Throttle);

  // Display should no longer need a redraw
  EXPECT_FALSE(mockDisplay->needsRedraw());
}

/**
 * @brief Test event type name is logged
 */
TEST_F(AppOrchestratorTests, TestOnEventTypeName) {
  // Set up the logger with an output stream to monitor
  Stream logStream;
  logger->setLogLevel(LogLevel::LOG_DEBUG);
  logger->setOutput(&logStream);

  // Create a dummy event
  EventData eventData;
  Event event = {EventType::CommandStationConnected, eventData};

  // Call onEvent which should trigger the debug message
  appOrchestrator->onEvent(event);

  // Check the output stream for the event type
  EXPECT_THAT(logStream.buffer, HasSubstr("CommandStationConnected"));
}
