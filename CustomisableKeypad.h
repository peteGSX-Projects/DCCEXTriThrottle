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

#ifndef CUSTOMISABLEKEYPAD_H
#define CUSTOMISABLEKEYPAD_H

// Do not use this for testing
#ifndef NATIVE_TESTING

#include "Defines.h"
#include "UserInputInterface.h"
#include <Arduino.h>

/**
 * @brief Implementation of the UserInputInterface using a flexible matrix keypad
 */
class CustomisableKeypad : public UserInputInterface {
public:
  /**
   * @brief Construct a new Advanced Keypad object
   * @param rows Number of rows
   * @param columns Number of columns
   * @param rowPins Pointer to the array of row pins
   * @param columnPins Pointer to the array of column pins
   * @param keyMap Pointer to the array of characters forming the key map
   * @param debounceTime Debounce time in ms
   * @param heldThreshold Time in ms that triggers a HELD event
   */
  CustomisableKeypad(byte numRows, byte numColumns, const byte *rowPins, const byte *columnPins, const char *keyMap,
                     unsigned long debounceTime, unsigned long heldThreshold);

  /**
   * @brief Initialise all keypad pins
   */
  void begin() override;

  /**
   * @brief Call checkKeypad() frequently to check for key press events
   * @return KeyEvent Key that is pressed, and the event type of the press
   */
  // KeyEvent checkKeypad();
  UserInputInterface::UserInputEvent check() override;

private:
  // Const attributes
  const byte _numRows;                /** Number of rows */
  const byte _numColumns;             /** Number of columns */
  const byte *_rowPins;               /** Pointer to array of row pins */
  const byte *_columnPins;            /** Pointer to array of column pins */
  const char *_keyMap;                /** Pointer to key map array */
  const unsigned long _debounceTime;  /** Debounce delay in ms */
  const unsigned long _heldThreshold; /** Time in ms to trigger HELD event */

  // Variable attributes
  char _activeKey;                 /** Key currently being processed */
  char _lastStableKey;             /** Last key held beyond debouce */
  unsigned long _lastDebounceTime; /** Time in ms of the last debounce */
  unsigned long _pressStartTime;   /** Track when a key is initially pressed */
  bool _isPressed;                 /** Current state of the hardware scan */
  bool _heldReported;              /** Flag for HELD reported for the current press*/

  /**
   * @brief Scan keypad using writes/reads to detect a key being pressed
   * @return char Key currently being pressed
   */
  char _scanKeypad();
};

#endif // NATIVE_TESTING

#endif // CUSTOMISABLEKEYPAD_H
