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
 * @brief Ensure the mock works to call display methods
 */
TEST_F(DisplayInterfaceTests, DisplayBasics) {
  // begin called once
  EXPECT_CALL(display, begin()).Times(1);
  display.begin();

  // clear called once
  EXPECT_CALL(display, clear()).Times(1);
  display.clear();

  // displayStartupInfo called with the right parameters
  const char *expectedHeader = "DCC-EX Tri-Throttle";
  const char *expectedVersion = VERSION;
  EXPECT_CALL(display, displayStartupScreen(StrEq(expectedHeader), StrEq(expectedVersion))).Times(1);
  display.displayStartupScreen("DCC-EX Tri-Throttle", VERSION);

  // Verify all calls happened as expected
  Mock::VerifyAndClearExpectations(&display);
}
