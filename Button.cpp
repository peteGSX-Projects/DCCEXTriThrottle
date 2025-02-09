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

#include "Button.h"

// Do not use this for testing
#ifndef PIO_UNIT_TESTING

Button::Button(byte pin, unsigned long debounceTime, unsigned long doubleClickTime, unsigned long longClickTime)
    : _pin(pin), _debounceTime(debounceTime), _doubleClickTime(doubleClickTime), _longClickTime(longClickTime) {
  _lastClickTime = 0;
  _clickStartTime = 0;
  _clickCount = 0;
  _isClicked = false;
  _longClickActivated = false;
}

void Button::begin() { pinMode(_pin, INPUT_PULLUP); }

Button::EventType Button::checkButton() {
  _updateButtonState();
  unsigned long currentTime = millis();

  if (_isClicked && (currentTime - _clickStartTime) > _longClickTime) {
    if (!_longClickActivated) {
      _longClickActivated = true;
      return {EventType::LongClick};
    }
    // Long click already reported, report held state
    return {EventType::Held};
  }

  if (!_isClicked) {
    EventType eventType;

    if (_longClickActivated) {
      // Key released after long click, don't report as single click
      _longClickActivated = false;
      _clickCount = 0;
      return {EventType::None};
    }

    if (_clickCount == 1 && (currentTime - _lastClickTime) > _doubleClickTime) {
      eventType = EventType::SingleClick;
    } else if (_clickCount == 2) {
      eventType = EventType::DoubleClick;
    } else {
      return {EventType::None};
    }
    _clickCount = 0;
    return {eventType};
  }

  return {EventType::None};
}

void Button::_updateButtonState() {
  bool currentState = digitalRead(_pin) == LOW;
  unsigned long currentTime = millis();
  if (currentState) {
    if (!_isClicked && (currentTime - _lastClickTime) > _debounceTime) {
      _isClicked = true;
      _clickStartTime = currentTime;
      if ((currentTime - _lastClickTime) < _doubleClickTime) {
        _clickCount++;
      } else {
        _clickCount = 1;
      }
      _lastClickTime = currentTime;
    }
  } else {
    if (_isClicked && (currentTime - _lastClickTime) > _debounceTime) {
      _isClicked = false;
    }
  }
}

#endif // PIO_UNIT_TESTING
