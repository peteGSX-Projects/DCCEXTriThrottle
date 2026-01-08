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
#include "Version.h"
#include "test/mocks/MockDisplay.h"
#include "test/mocks/MockKeypad.h"
#include <gtest/gtest.h>

using namespace testing;

/// @brief Test harness for AppOrchestrator
class AppOrchestratorTests : public Test {
protected:
  AppOrchestrator *appOrchestrator;
  MockDisplay *mockDisplay;
  MockKeypad *mockKeypad;
  Logger *logger;

  void SetUp() override {
    mockDisplay = new MockDisplay;
    mockKeypad = new MockKeypad;
    logger = new Logger;
    appOrchestrator = new AppOrchestrator(mockDisplay, mockKeypad, logger);
  }

  void TearDown() override {
    delete appOrchestrator;
    delete mockDisplay;
    delete mockKeypad;
    delete logger;
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
