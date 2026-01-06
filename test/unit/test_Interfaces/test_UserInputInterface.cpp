/*
 *  © 2026 Peter Cole
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
 * @brief Test a single key press event
 */
TEST_F(UserInputInterfaceTests, ReturnsPressedEvent) {
  UserInputInterface::UserInputEvent setEvent = {'1', UserInputInterface::UserInputAction::Pressed};
  keypad.setInputEvent(setEvent);

  UserInputInterface::UserInputEvent event = keypad.check();
  EXPECT_EQ(event.key, '1');
  EXPECT_EQ(event.action, UserInputInterface::UserInputAction::Pressed);
}

/**
 * @brief Test a key held event
 */
TEST_F(UserInputInterfaceTests, ReturnsHeldEvent) {
  UserInputInterface::UserInputEvent setEvent = {'5', UserInputInterface::UserInputAction::Held};
  keypad.setInputEvent(setEvent);

  UserInputInterface::UserInputEvent event = keypad.check();
  EXPECT_EQ(event.key, '5');
  EXPECT_EQ(event.action, UserInputInterface::UserInputAction::Held);
}

/**
 * @brief Test a key released event
 */
TEST_F(UserInputInterfaceTests, ReturnsReleasedEvent) {
  UserInputInterface::UserInputEvent setEvent = {'*', UserInputInterface::UserInputAction::Released};
  keypad.setInputEvent(setEvent);

  UserInputInterface::UserInputEvent event = keypad.check();
  EXPECT_EQ(event.key, '*');
  EXPECT_EQ(event.action, UserInputInterface::UserInputAction::Released);
}
