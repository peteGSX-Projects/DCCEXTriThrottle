/*
 *  © 2025 Peter Cole
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

#include "Version.h"
#include "test/mocks/MockDisplay.h"
#include <gtest/gtest.h>

using namespace testing;

class DisplayInterfaceTests : public Test {
protected:
  MockDisplay display;

  void SetUp() override {}

  void TearDown() override {}
};

/**
 * @brief Test the flow of the progress screen
 */
TEST_F(DisplayInterfaceTests, testProgressScreenFlow) {
  // Set expectations
  EXPECT_CALL(display, displayProgressScreen(StrEq("Startup"), StrEq("Connecting..."))).Times(1);
  EXPECT_CALL(display, updateProgressScreen()).Times(3);

  // Call the methods
  display.displayProgressScreen("Startup", "Connecting...");
  display.updateProgressScreen();
  display.updateProgressScreen();
  display.updateProgressScreen();

  Mock::VerifyAndClearExpectations(&display);
}
