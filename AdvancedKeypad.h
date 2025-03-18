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

#ifndef ADVANCEDKEYPAD_H
#define ADVANCEDKEYPAD_H

// Do not use this for testing
#ifndef NATIVE_TESTING

#include <Arduino.h>

/**
 * @brief Define default keypad pins if not customised elsewhere
 */
#ifndef KEYPAD_PIN1
#define KEYPAD_PIN1 PB3
#endif // KEYPAD_PIN1
#ifndef KEYPAD_PIN2
#define KEYPAD_PIN2 PB4
#endif // KEYPAD_PIN2
#ifndef KEYPAD_PIN3
#define KEYPAD_PIN3 PB5
#endif // KEYPAD_PIN3
#ifndef KEYPAD_PIN4
#define KEYPAD_PIN4 PB6
#endif // KEYPAD_PIN4
#ifndef KEYPAD_PIN5
#define KEYPAD_PIN5 PB7
#endif // KEYPAD_PIN5
#ifndef KEYPAD_PIN6
#define KEYPAD_PIN6 PB8
#endif // KEYPAD_PIN6
#ifndef KEYPAD_PIN7
#define KEYPAD_PIN7 PB9
#endif // KEYPAD_PIN7

/**
 * @brief Define debounce, double press, and hold times
 */
#ifndef KEYPAD_DEBOUNCE_TIME
#define KEYPAD_DEBOUNCE_TIME 20
#endif // KEYPAD_DEBOUNCE_TIME
#ifndef KEYPAD_DOUBLE_PRESS_TIME
#define KEYPAD_DOUBLE_PRESS_TIME 200
#endif // KEYPAD_DOUBLE_PRESS_TIME
#ifndef KEYPAD_LONG_PRESS_TIME
#define KEYPAD_LONG_PRESS_TIME 500
#endif // KEYPAD_LONG_PRESS_TIME

class AdvancedKeypad {
public:
  /**
   * @brief Event types available for key presses
   */
  enum class EventType { None, SinglePress, DoublePress, LongPress };

  /**
   * @brief Structure for a key press event to get both the key pressed and the event type
   */
  struct KeyEvent {
    char key;
    EventType type;
  };

  /**
   * @brief Construct a new Advanced Keypad object
   * @param keypadPin1 Pin the first keypad row pin is connected to
   * @param keypadPin2 Pin the second keypad row pin is connected to
   * @param keypadPin3 Pin the third keypad row pin is connected to
   * @param keypadPin4 Pin the fourth keypad row pin is connected to
   * @param keypadPin5 Pin the first keypad column pin is connected to
   * @param keypadPin6 Pin the second keypad column pin is connected to
   * @param keypadPin7 Pin the third keypad column pin is connected to
   * @param debounceTime Debounce time in ms, adjust to suit the physical keypad (default 20ms)
   * @param doublePressTime Time in ms in which a double press must occur (default 200ms)
   * @param longPressTime Time in ms a key must be held to flag a long press (default 500ms)
   */
  AdvancedKeypad(byte keypadPin1 = KEYPAD_PIN1, byte keypadPin2 = KEYPAD_PIN2, byte keypadPin3 = KEYPAD_PIN3,
                 byte keypadPin4 = KEYPAD_PIN4, byte keypadPin5 = KEYPAD_PIN5, byte keypadPin6 = KEYPAD_PIN6,
                 byte keypadPin7 = KEYPAD_PIN7, unsigned long debounceTime = KEYPAD_DEBOUNCE_TIME,
                 unsigned long doublePressTime = KEYPAD_DOUBLE_PRESS_TIME,
                 unsigned long longPressTime = KEYPAD_LONG_PRESS_TIME);

  /**
   * @brief Initialise all keypad pins
   */
  void begin();

  /**
   * @brief Call checkKeypad() frequently to check for key press events
   * @return KeyEvent Key that is pressed, and the event type of the press
   */
  KeyEvent checkKeypad();

private:
  // Const attributes
  const byte _rowPins[4];               /** Array of pin numbers the rows are connected to */
  const byte _columnPins[3];            /** Array of pin numbers the columns are connected to */
  const unsigned long _debounceTime;    /** Debounce delay in ms */
  const unsigned long _doublePressTime; /** Time in ms in which a double press must occur */
  const unsigned long _longPressTime;   /** Time in ms to hold to record a long press */

  const char _keymap[4][3] = {
      /** Key mappings */
      {'1', '2', '3'}, /** Row 1 */
      {'4', '5', '6'}, /** Row 2 */
      {'7', '8', '9'}, /** Row 3 */
      {'*', '0', '#'}  /** Row 4 */
  };

  // Variable attributes
  char _lastKey;                 /** Track the last key pressed for debouncing etc. */
  unsigned long _lastPressTime;  /** Time of last press for calculations */
  unsigned long _pressStartTime; /** Time press started for calculations */
  uint8_t _pressCount;           /** Number of times pressed for calculations */
  bool _isPressed;               /** Track if pressed for calculations */
  bool _longPressActivated;      /** Track to prevent repetitive long press events */

  /**
   * @brief Scan keypad using writes/reads to detect a key being pressed
   * @return char Key currently being pressed
   */
  char _scanKeypad();

  /**
   * @brief Update the current keypad state including debouncing and key press count
   */
  void _updateKeypadState();
};

#endif // NATIVE_TESTING

#endif // ADVANCEDKEYPAD_H
