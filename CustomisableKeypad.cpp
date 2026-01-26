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

#include "CustomisableKeypad.h"

// Do not use when testing
#ifndef NATIVE_TESTING

CustomisableKeypad::CustomisableKeypad(byte numRows, byte numColumns, const byte *rowPins, const byte *columnPins,
                                       const char *keyMap, unsigned long debounceTime, unsigned long heldThreshold)
    : _numRows(numRows), _numColumns(numColumns), _rowPins(rowPins), _columnPins(columnPins), _keyMap(keyMap),
      _debounceTime(debounceTime), _heldThreshold(heldThreshold) {
  _activeKey = '\0';
  _lastStableKey = '\0';
  _lastDebounceTime = 0;
  _isPressed = false;
  _heldReported = false;
}

void CustomisableKeypad::begin() {
  for (int row = 0; row < _numRows; row++) {
    pinMode(_rowPins[row], OUTPUT);    // Rows in output mode
    digitalWrite(_rowPins[row], HIGH); // Default HIGH state
  }
  for (int column = 0; column < _numColumns; column++) {
    pinMode(_columnPins[column], INPUT_PULLUP); // Columns in input mode with pullups
  }
  LOG(LogLevel::LOG_DEBUG, "CustomisableKeypad::begin(): initialise with rows: ", _numRows);
  LOG(LogLevel::LOG_DEBUG, "CustomisableKeypad::begin(): initialise with columns: ", _numColumns);
}

UserInputInterface::UserInputEvent CustomisableKeypad::check() {
  char scannedKey = _scanKeypad();
  unsigned long currentTime = millis();

  // Handle debounce first
  if (scannedKey != _activeKey) {
    _lastDebounceTime = currentTime;
    _activeKey = scannedKey;
  }

  if ((currentTime - _lastDebounceTime) > _debounceTime) {
    // First check if a key has been pressed
    if (_activeKey != '\0' && !_isPressed) {
      _isPressed = true;
      _lastStableKey = _activeKey;
      _heldReported = false;
      _pressStartTime = currentTime; // Track key press start
    }
  }

  // Check if it is Held first
  if (_isPressed && _activeKey == _lastStableKey) {
    if (!_heldReported && (currentTime - _pressStartTime) > _heldThreshold) {
      _heldReported = true;
      LOG(LogLevel::LOG_DEBUG, "Key Held: ", _lastStableKey);
      return {_lastStableKey, UserInputAction::Held};
    }
  }

  // Check for a release
  if (_isPressed && _activeKey == '\0') {
    _isPressed = false;
    char releasedKey = _lastStableKey;
    _lastStableKey = '\0';

    if (_heldReported) {
      // It was Held, therefore this must be a Release
      LOG(LogLevel::LOG_DEBUG, "Key Released: ", releasedKey);
      return {releasedKey, UserInputAction::Released};
    } else {
      // Otherwise it is Pressed
      LOG(LogLevel::LOG_DEBUG, "Key Pressed: ", releasedKey);
      return {releasedKey, UserInputAction::Pressed};
    }
  }

  return {'\0', UserInputAction::None}; // Return null terminator if we reached here
}

char CustomisableKeypad::_scanKeypad() {
  for (int row = 0; row < _numRows; row++) {
    digitalWrite(_rowPins[row], LOW);
    for (int column = 0; column < _numColumns; column++) {
      if (digitalRead(_columnPins[column]) == LOW) {
        digitalWrite(_rowPins[row], HIGH);
        // Calculate key map index from coordinates
        return _keyMap[row * _numColumns + column];
      }
    }
    digitalWrite(_rowPins[row], HIGH);
  }
  return '\0';
}

#endif // NATIVE_TESTING
