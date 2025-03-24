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

#include "test/mocks/MockKeypad.h"
#include <gtest/gtest.h>

using namespace testing;

class UserInputInterfaceTests : public Test {
protected:
  MockKeypad keypad;

  void SetUp() override {}

  void TearDown() override {}
};

/**
 * @brief Ensure the mock works to set and retrieve a proper keypad event
 */
TEST_F(UserInputInterfaceTests, CheckEventRetrieval) {
  UserInputInterface::UserInputEvent setEvent = {'1', UserInputInterface::UserInputAction::SinglePress};
  keypad.setInputEvent(setEvent);
  UserInputInterface::UserInputEvent event = keypad.check();
  EXPECT_EQ(setEvent.key, event.key);
  EXPECT_EQ(setEvent.action, setEvent.action);
}
