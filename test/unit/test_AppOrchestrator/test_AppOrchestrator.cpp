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
    appOrchestrator = new AppOrchestrator(mockDisplay, mockKeypad, logger);
  }

  void TearDown() override {
    delete appOrchestrator;
    delete mockDisplay;
    delete mockKeypad;
    delete logger;
  }
};
