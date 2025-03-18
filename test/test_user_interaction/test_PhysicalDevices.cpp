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

#ifndef NATIVE_TESTING

#include "AdvancedKeypad.h"
#include "Button.h"
#include "RotaryEncoder.h"
#include <Arduino.h>
#include <unity.h>

AdvancedKeypad keypad;
Button button1(ENCODER1_BUTTON);
Button button2(ENCODER2_BUTTON);
Button button3(ENCODER3_BUTTON);
RotaryEncoder encoder1(ENCODER1_DT, ENCODER1_CLK, RotaryEncoder::Mode::FullStep);
RotaryEncoder encoder2(ENCODER2_DT, ENCODER2_CLK, RotaryEncoder::Mode::FullStep);
RotaryEncoder encoder3(ENCODER3_DT, ENCODER3_CLK, RotaryEncoder::Mode::FullStep);

void test_keypad_initialisation() {
  TEST_ASSERT_TRUE(keypad.begin());
}

void setup() {
  UNITY_BEGIN();
  RUN_TEST(test_keypad_initialisation);
}

void loop() {}

#endif // NATIVE_TESTING
