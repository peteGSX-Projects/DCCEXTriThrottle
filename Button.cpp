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
#ifndef NATIVE_TESTING

Button::Button(byte pin, unsigned long debounceTime, unsigned long doubleClickTime, unsigned long longClickTime)
    : _pin(pin), _debounceTime(debounceTime), _doubleClickTime(doubleClickTime), _longClickTime(longClickTime) {
  _lastClickTime = 0;
  _clickStartTime = 0;
  _clickCount = 0;
  _isClicked = false;
  _longClickActivated = false;
}

void Button::begin() { pinMode(_pin, INPUT_PULLUP); }

UserConfirmationInterface::UserConfirmationAction Button::check() {
  _updateButtonState();
  unsigned long currentTime = millis();

  if (_isClicked && (currentTime - _clickStartTime) > _longClickTime) {
    if (!_longClickActivated) {
      _longClickActivated = true;
      return {UserConfirmationAction::LongClick};
    }
    // Long click already reported, report held state
    return {UserConfirmationAction::Held};
  }

  if (!_isClicked) {
    UserConfirmationAction action;

    if (_longClickActivated) {
      // Key released after long click, don't report as single click
      _longClickActivated = false;
      _clickCount = 0;
      return {UserConfirmationAction::None};
    }

    if (_clickCount == 1 && (currentTime - _lastClickTime) > _doubleClickTime) {
      action = UserConfirmationAction::SingleClick;
    } else if (_clickCount == 2) {
      action = UserConfirmationAction::DoubleClick;
    } else {
      return {UserConfirmationAction::None};
    }
    _clickCount = 0;
    return {action};
  }

  return {UserConfirmationAction::None};
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

#endif // NATIVE_TESTING
