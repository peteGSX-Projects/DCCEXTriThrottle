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

#include "AdvancedKeypad.h"

AdvancedKeypad::AdvancedKeypad(byte keypadPin1, byte keypadPin2, byte keypadPin3, byte keypadPin4, byte keypadPin5,
                               byte keypadPin6, byte keypadPin7, unsigned long debounceTime,
                               unsigned long doublePressTime, unsigned long longPressTime)
    : _rowPins{keypadPin2, keypadPin7, keypadPin6, keypadPin4}, _columnPins{keypadPin3, keypadPin1, keypadPin5},
      _debounceTime(debounceTime), _doublePressTime(doublePressTime), _longPressTime(longPressTime) {}

void AdvancedKeypad::begin() {
  for (int i = 0; i < 4; i++) {
    pinMode(_rowPins[i], OUTPUT);
  }
  for (int i = 0; i < 3; i++) {
    pinMode(_columnPins[i], INPUT_PULLUP);
  }
}

AdvancedKeypad::KeyEvent AdvancedKeypad::checkKeypad() {
  _updateKeypadState();
  unsigned long currentTime = millis();
  if (!_isPressed && _lastKey != '\0') {
    if (_pressCount == 1 && (currentTime - _lastPressTime) > _doublePressTime) {
      char key = _lastKey;
      _lastKey = 0;
      return {key, EventType::SinglePress};
    } else if (_pressCount == 2) {
      char key = _lastKey;
      _lastKey = 0;
      _pressCount = 0;
      return {key, EventType::DoublePress};
    }
  } else if (_isPressed && (currentTime - _pressStartTime) > _longPressTime) {
    return {_lastKey, EventType::LongPress};
  }
  return {'\0', EventType::None};
}

char AdvancedKeypad::_scanKeypad() {
  for (int row = 0; row < 4; row++) {
    digitalWrite(_rowPins[row], LOW);
    for (int column = 0; column < 3; column++) {
      if (digitalRead(_columnPins[column]) == LOW) {
        digitalWrite(_rowPins[row], HIGH);
        return _keymap[row][column];
      }
    }
    digitalWrite(_rowPins[row], HIGH);
  }
  return '\0';
}

void AdvancedKeypad::_updateKeypadState() {
  char currentKey = _scanKeypad();
  unsigned long currentTime = millis();
  if (currentKey != 0) {
    if (!_isPressed && (currentTime - _lastPressTime) > _debounceTime) {
      _isPressed = true;
      _pressStartTime = currentTime;
      if (currentKey == _lastKey && (currentTime - _lastPressTime) < _doublePressTime) {
        _pressCount++;
      } else {
        _pressCount = 1;
      }
      _lastKey = currentKey;
      _lastPressTime = currentTime;
    }
  } else {
    if (_isPressed && (currentTime - _lastPressTime) > _debounceTime) {
      _isPressed = false;
    }
  }
}
