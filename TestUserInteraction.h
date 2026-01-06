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

#ifndef TESTUSERINTERACTION_H
#define TESTUSERINTERACTION_H

#include "Defines.h"
#include "Logger.h"
#include "Version.h"

// Don't include this if doing native testing
#if !defined(NATIVE_TESTING) && defined(DEVICE_TESTING)

#include "Button.h"
#include "CustomisableKeypad.h"
#include "RotaryEncoder.h"
#include "U8G2SH1106Display.h"
#include <Arduino.h>

// Define the required keypad arrays using user defines
static const byte testRowPins[] = {KEYPAD_ROW_PINS};
static const byte testColumnPins[] = {KEYPAD_COLUMN_PINS};
static const char testKeyMap[] = {KEYPAD_MAP};

CustomisableKeypad keypad(KEYPAD_ROWS, KEYPAD_COLUMNS, testRowPins, testColumnPins, testKeyMap, KEYPAD_DEBOUNCE_TIME,
                          KEYPAD_HELD_THRESHOLD);
Button button1(ENCODER1_BUTTON);
Button button2(ENCODER2_BUTTON);
Button button3(ENCODER3_BUTTON);
RotaryEncoder encoder1(ENCODER1_DT, ENCODER1_CLK, RotaryEncoder::Mode::FullStep);
RotaryEncoder encoder2(ENCODER2_DT, ENCODER2_CLK, RotaryEncoder::Mode::FullStep);
RotaryEncoder encoder3(ENCODER3_DT, ENCODER3_CLK, RotaryEncoder::Mode::FullStep);
U8G2SH1106Display display;

#define TEST_START Logger::log(LogLevel::LOG_MESSAGE, "--- TEST START ---");
#define TEST_END Logger::log(LogLevel::LOG_MESSAGE, "--- TEST END ---\n");

/**
 * @brief Perform basic display testing showing test message and software version
 */
void displayTest() {
  TEST_START
  LOG(LogLevel::LOG_MESSAGE, "Display testing");
  display.begin();
  display.clear();
  display.displayStartupScreen("Display testing", VERSION);
  TEST_END
}

/// @brief Prompt the user to interact in the specified way and validate the correct KeyEvent is returned
/// @param prompt Message telling the user which key to use, and whether to press, double press, or hold
/// @param expectedKey Key to use
/// @param expectedType EventType
/// @param timeout Milliseconds to wait before failing test
void promptKeypadTest(const char *prompt, char expectedKey, UserInputInterface::UserInputAction expectedType,
                      unsigned long timeout) {
  TEST_START
  LOG(LogLevel::LOG_MESSAGE, prompt);
  timeout = timeout + millis();

  // Poll and wait for a valid event type
  UserInputInterface::UserInputEvent event;
  do {
    event = keypad.check();
  } while (event.action == UserInputInterface::UserInputAction::None && millis() < timeout);

  // Display test output in the console
  bool testPassed = (event.key == expectedKey && event.action == expectedType);

  LogLevel level;

  if (testPassed) {
    level = LogLevel::LOG_MESSAGE;
  } else {
    level = LogLevel::LOG_ERROR;
  }
  if (millis() >= timeout) {
    LOG(level, "Test timed out");
  } else {
    LOG(level, "expectedKey|event.key|expectedType|event.type: %c|%c|%d|%d", expectedKey, event.key,
        static_cast<int>(expectedType), static_cast<int>(event.action));
  }
  TEST_END
}

/// @brief Prompt the user to rotate the specified encoder in the provided direction for the number of steps
/// @param prompt Message telling the user which encoder to rotate, in which direction, and for how many steps
/// @param encoder Pointer to the RotaryEncoder to test
/// @param expectedDirection Direction for the user to rotate it
/// @param expectedSteps Number of steps to rotate
/// @param timeout Milliseconds to wait before failing test
void promptEncoderTest(const char *prompt, RotaryEncoder *encoder, RotaryEncoder::Direction expectedDirection,
                       int expectedSteps, unsigned long timeout) {
  TEST_START
  LOG(LogLevel::LOG_MESSAGE, prompt);
  timeout = timeout + millis();
  int counter = 0;
  RotaryEncoder::Direction direction;

  // Poll and wait for the correct number of steps
  do {
    direction = encoder->checkDirection();
    if (direction == expectedDirection) {
      counter++;
    } else if (direction != RotaryEncoder::Direction::None) {
      counter--;
    }
  } while (counter < expectedSteps && millis() < timeout);

  // Display test output in the console
  bool testPassed = (counter == expectedSteps);

  LogLevel level;

  if (testPassed) {
    level = LogLevel::LOG_MESSAGE;
  } else {
    level = LogLevel::LOG_ERROR;
  }
  if (millis() >= timeout) {
    LOG(level, "Test timed out");
  } else {
    LOG(level, "expectedSteps|counter: %d|%d", expectedSteps, counter);
  }
  TEST_END
}

/// @brief Prompt the user to use the specified button to test the specified EventType
/// @param prompt Message telling the user which button to press, and what type of press to perform
/// @param button Pointer to the Button to test
/// @param expectedAction Expected UserConfirmationAction
/// @param timeout Milliseconds to wait before failing test
void promptButtonTest(const char *prompt, Button *button,
                      UserConfirmationInterface::UserConfirmationAction expectedAction, unsigned long timeout) {
  TEST_START
  LOG(LogLevel::LOG_MESSAGE, prompt);
  timeout = timeout + millis();
  UserConfirmationInterface::UserConfirmationAction action;

  // Poll and wait for the correct event type
  do {
    action = button->check();
  } while (action == UserConfirmationInterface::UserConfirmationAction::None && millis() < timeout);

  // Display test output in the console
  bool testPassed = (action == expectedAction);

  LogLevel level;

  if (testPassed) {
    level = LogLevel::LOG_MESSAGE;
  } else {
    level = LogLevel::LOG_ERROR;
  }
  if (millis() >= timeout) {
    LOG(level, "Test timed out");
  } else {
    LOG(level, "expectedAction|action: %d|%d", static_cast<int>(expectedAction), static_cast<int>(action));
  }
  TEST_END
}

void setup() {
  CONSOLE_STREAM.begin(115200);
  Logger::setOutput(&CONSOLE_STREAM);
  delay(5000);
  LOG(LogLevel::LOG_MESSAGE, "DCC-EX TriThrottle Testing");
#ifdef LOG_LEVEL
  Logger::setLogLevel(LOG_LEVEL);
#endif // ENABLE_DEBUG
  display.begin();
  keypad.begin();
  encoder1.begin();
  encoder2.begin();
  encoder3.begin();
  button1.begin();
  button2.begin();
  button3.begin();

#ifdef TEST_DISPLAY
  displayTest();
#endif // TEST_DISPLAY

#ifdef TEST_KEYPAD
  // Test Single press 1
  promptKeypadTest("Press the '1' key briefly", '1', UserInputInterface::UserInputAction::Pressed, 5000);

  // Test Held *
  promptKeypadTest("Press and hold the '*' key", '*', UserInputInterface::UserInputAction::Held, 5000);

  // Test Released *
  promptKeypadTest("Now release the '*' key", '*', UserInputInterface::UserInputAction::Released, 5000);

  // Test different key for Held/Released - 7
  promptKeypadTest("Press and hold the '7' key", '7', UserInputInterface::UserInputAction::Held, 5000);
  promptKeypadTest("Now release the '7' key", '7', UserInputInterface::UserInputAction::Released, 5000);
#endif // TEST_KEYPAD

#ifdef TEST_ENCODERS
  // Test encoder1 2 steps CW, 5 steps CCW
  promptEncoderTest("Rotate encoder 1 clockwise 2 steps", &encoder1, RotaryEncoder::Direction::CW, 2, 5000);
  promptEncoderTest("Rotate encoder 1 counter clockwise 5 steps", &encoder1, RotaryEncoder::Direction::CCW, 5, 5000);

  // Test encoder2 8 steps CW, 3 steps CCW
  promptEncoderTest("Rotate encoder 2 clockwise 8 steps", &encoder2, RotaryEncoder::Direction::CW, 8, 5000);
  promptEncoderTest("Rotate encoder 2 counter clockwise 3 steps", &encoder2, RotaryEncoder::Direction::CCW, 3, 5000);

  // Test encoder3 2 steps CW, 5 steps CCW
  promptEncoderTest("Rotate encoder 3 clockwise 2 steps", &encoder3, RotaryEncoder::Direction::CW, 2, 5000);
  promptEncoderTest("Rotate encoder 3 counter clockwise 5 steps", &encoder3, RotaryEncoder::Direction::CCW, 5, 5000);
#endif // TEST_ENCODERS

#ifdef TEST_BUTTONS
  // Test button1 single click, double click, long press
  promptButtonTest("Press button 1 once", &button1, UserConfirmationInterface::UserConfirmationAction::SingleClick,
                   5000);
  promptButtonTest("Double click button 1", &button1, UserConfirmationInterface::UserConfirmationAction::DoubleClick,
                   5000);
  promptButtonTest("Long press button 1", &button1, UserConfirmationInterface::UserConfirmationAction::LongClick, 5000);

  // Test button2 single click, double click, long press
  promptButtonTest("Press button 2 once", &button2, UserConfirmationInterface::UserConfirmationAction::SingleClick,
                   5000);
  promptButtonTest("Double click button 2", &button2, UserConfirmationInterface::UserConfirmationAction::DoubleClick,
                   5000);
  promptButtonTest("Long press button 2", &button2, UserConfirmationInterface::UserConfirmationAction::LongClick, 5000);

  // Test button3 single click, double click, long press
  promptButtonTest("Press button 3 once", &button3, UserConfirmationInterface::UserConfirmationAction::SingleClick,
                   5000);
  promptButtonTest("Double click button 3", &button3, UserConfirmationInterface::UserConfirmationAction::DoubleClick,
                   5000);
  promptButtonTest("Long press button 3", &button3, UserConfirmationInterface::UserConfirmationAction::LongClick, 5000);
#endif // TEST_BUTTONS
}

void loop() {}

#endif // !defined(NATIVE_TESTING) && defined(DEVICE_TESTING)

#endif // TESTUSERINTERACTION_H
