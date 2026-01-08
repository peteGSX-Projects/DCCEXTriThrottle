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
 * @brief Ensure AppOrchestrator goes from Startup to Throttle when any key event occurs
 */
TEST_F(AppOrchestratorTests, ValidateStartuptoThrottleState) {
  const char *expectedText = "DCC-EX Tri-Throttle";
  const char *expectedVersion = VERSION;
  
  // Validate starts in Startup
  EXPECT_EQ(appOrchestrator->getCurrentAppState(), AppState::Startup);

  // Startup display should be called exactly twice
  EXPECT_CALL(*mockDisplay, displayStartupScreen(StrEq(expectedText), StrEq(expectedVersion))).Times(2);
  
  // Single update with no key presses should remain in Startup, and display startup should be called
  appOrchestrator->update();
  EXPECT_EQ(appOrchestrator->getCurrentAppState(), AppState::Startup);

  // Key press should change to Throttle
  mockKeypad->setInputEvent({'1', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();
  EXPECT_EQ(appOrchestrator->getCurrentAppState(), AppState::Throttle);

  // Subsequent update with no user input should maintain state and not call startup display again
  appOrchestrator->update();
  EXPECT_EQ(appOrchestrator->getCurrentAppState(), AppState::Throttle);
}
