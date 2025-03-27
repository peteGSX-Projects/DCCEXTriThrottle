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

#include "Defines.h"

// Don't include this if doing native testing
#if !defined(NATIVE_TESTING) && defined(DEVICE_TESTING)

#include "AdvancedKeypad.h"
#include "Button.h"
#include "RotaryEncoder.h"
#include <Arduino.h>

AdvancedKeypad keypad;
Button button1(ENCODER1_BUTTON);
Button button2(ENCODER2_BUTTON);
Button button3(ENCODER3_BUTTON);
RotaryEncoder encoder1(ENCODER1_DT, ENCODER1_CLK, RotaryEncoder::Mode::FullStep);
RotaryEncoder encoder2(ENCODER2_DT, ENCODER2_CLK, RotaryEncoder::Mode::FullStep);
RotaryEncoder encoder3(ENCODER3_DT, ENCODER3_CLK, RotaryEncoder::Mode::FullStep);

/// @brief Prompt the user to interact in the specified way and validate the correct KeyEvent is returned
/// @param prompt Message telling the user which key to use, and whether to press, double press, or hold
/// @param expectedKey Key to use
/// @param expectedType EventType
/// @param timeout Milliseconds to wait before failing test
void promptKeypadTest(const char *prompt, char expectedKey, AdvancedKeypad::EventType expectedType,
                      unsigned long timeout) {
  Serial.println(prompt);
  timeout = timeout + millis();

  // Poll and wait for a valid event type
  AdvancedKeypad::KeyEvent event;
  do {
    event = keypad.checkKeypad();
  } while (event.type == AdvancedKeypad::EventType::None && millis() < timeout);

  // Display test output in the console
  bool testPassed = (event.key == expectedKey && event.type == expectedType);

  if (testPassed) {
    Serial.print("Test passed: ");
  } else {
    Serial.print("Test FAILED: ");
  }
  Serial.print("expectedKey|event.key|expectedType|event.type: ");
  Serial.print(expectedKey);
  Serial.print("|");
  Serial.print(event.key);
  Serial.print("|");
  Serial.print(static_cast<int>(expectedType));
  Serial.print("|");
  Serial.println(static_cast<int>(event.type));
}

/// @brief Prompt the user to rotate the specified encoder in the provided direction for the number of steps
/// @param prompt Message telling the user which encoder to rotate, in which direction, and for how many steps
/// @param encoder Pointer to the RotaryEncoder to test
/// @param expectedDirection Direction for the user to rotate it
/// @param expectedSteps Number of steps to rotate
/// @param timeout Milliseconds to wait before failing test
void promptEncoderTest(const char *prompt, RotaryEncoder *encoder, RotaryEncoder::Direction expectedDirection,
                       int expectedSteps, unsigned long timeout) {
  Serial.println(prompt);
  timeout = timeout + millis();
  int counter = 0;
  RotaryEncoder::Direction direction;

  // Poll and wait for the correct number of steps
  do {
    direction = encoder->check();
    if (direction == expectedDirection) {
      counter++;
    } else if (direction != RotaryEncoder::Direction::None) {
      counter--;
    }
  } while (counter < expectedSteps && millis() < timeout);
}

/// @brief Prompt the user to use the specified button to test the specified EventType
/// @param prompt Message telling the user which button to press, and what type of press to perform
/// @param button Pointer to the Button to test
/// @param expectedAction Expected UserConfirmationAction
/// @param timeout Milliseconds to wait before failing test
void promptButtonTest(const char *prompt, Button *button,
                      UserConfirmationInterface::UserConfirmationAction expectedAction, unsigned long timeout) {
  Serial.println(prompt);
  timeout = timeout + millis();
  UserConfirmationInterface::UserConfirmationAction action;

  // Poll and wait for the correct event type
  do {
    action = button->check();
  } while (action == UserConfirmationInterface::UserConfirmationAction::None && millis() < timeout);

  // Display test output in the console
  bool testPassed = (action == expectedAction);

  if (testPassed) {
    Serial.print("Test passed: ");
  } else {
    Serial.print("Test FAILED: ");
  }
  Serial.print("expectedAction|action: ");
  Serial.print(static_cast<int>(expectedAction));
  Serial.print("|");
  Serial.println(static_cast<int>(action));
}

void setup() {
  Serial.begin(115200);
  delay(3000);
  keypad.begin();
  encoder1.begin();
  encoder2.begin();
  encoder3.begin();
  button1.begin();
  button2.begin();
  button3.begin();

#ifdef TEST_DISPLAY
  Serial.println("Display testing starts now");
  Serial.println("Display testing ended");
#endif // TEST_DISPLAY

#ifdef TEST_KEYPAD
  // Test 1 - Single press 1
  promptKeypadTest("Press the number 1 briefly", '1', AdvancedKeypad::EventType::SinglePress, 5000);

  // Test 2 - Double press 2
  promptKeypadTest("Double Press the number 2", '2', AdvancedKeypad::EventType::DoublePress, 5000);

  // Test 3 - Long press #
  promptKeypadTest("Press and hold the # key", '#', AdvancedKeypad::EventType::LongPress, 5000);
#endif // TEST_KEYPAD

#ifdef TEST_ENCODERS
  // Test 4 and 5 - encoder1 2 steps CW, 5 steps CCW
  promptEncoderTest("Rotate encoder 1 clockwise 2 steps", &encoder1, RotaryEncoder::Direction::CW, 2, 5000);
  promptEncoderTest("Rotate encoder 1 counter clockwise 5 steps", &encoder1, RotaryEncoder::Direction::CCW, 5, 5000);

  // Test 6 and 7 - encoder2 8 steps CW, 3 steps CCW
  promptEncoderTest("Rotate encoder 2 clockwise 8 steps", &encoder2, RotaryEncoder::Direction::CW, 8, 5000);
  promptEncoderTest("Rotate encoder 2 counter clockwise 3 steps", &encoder2, RotaryEncoder::Direction::CCW, 3, 5000);

  // Test 8 and 9 - encoder3 2 steps CW, 5 steps CCW
  promptEncoderTest("Rotate encoder 3 clockwise 2 steps", &encoder3, RotaryEncoder::Direction::CW, 2, 5000);
  promptEncoderTest("Rotate encoder 3 counter clockwise 5 steps", &encoder3, RotaryEncoder::Direction::CCW, 5, 5000);
#endif // TEST_ENCODERS

#ifdef TEST_BUTTONS
  // Test 10, 11, 12, 13 - button1 single click, double click, long press, held
  promptButtonTest("Press button 1 once", &button1, UserConfirmationInterface::UserConfirmationAction::SingleClick,
                   5000);
  promptButtonTest("Double click button 1", &button1, UserConfirmationInterface::UserConfirmationAction::DoubleClick,
                   5000);
  promptButtonTest("Long press button 1", &button1, UserConfirmationInterface::UserConfirmationAction::LongClick, 5000);
  promptButtonTest("Press and hold button 1", &button1, UserConfirmationInterface::UserConfirmationAction::Held, 5000);

  // Test 14, 15, 16, 17 - button2 single click, double click, long press, held
  promptButtonTest("Press button 2 once", &button2, UserConfirmationInterface::UserConfirmationAction::SingleClick,
                   5000);
  promptButtonTest("Double click button 2", &button2, UserConfirmationInterface::UserConfirmationAction::DoubleClick,
                   5000);
  promptButtonTest("Long press button 2", &button2, UserConfirmationInterface::UserConfirmationAction::LongClick, 5000);
  promptButtonTest("Press and hold button 2", &button2, UserConfirmationInterface::UserConfirmationAction::Held, 5000);

  // Test 18, 19, 20, 21 - button3 single click, double click, long press, held
  promptButtonTest("Press button 3 once", &button3, UserConfirmationInterface::UserConfirmationAction::SingleClick,
                   5000);
  promptButtonTest("Double click button 3", &button3, UserConfirmationInterface::UserConfirmationAction::DoubleClick,
                   5000);
  promptButtonTest("Long press button 3", &button3, UserConfirmationInterface::UserConfirmationAction::LongClick, 5000);
  promptButtonTest("Press and hold button 3", &button3, UserConfirmationInterface::UserConfirmationAction::Held, 5000);
#endif // TEST_BUTTONS
}

void loop() {}

#endif // !defined(NATIVE_TESTING) && defined(DEVICE_TESTING)
