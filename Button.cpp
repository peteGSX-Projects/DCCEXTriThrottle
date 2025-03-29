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

Button::Button(byte pin) {
  _button = new Switch(pin, BUTTON_PIN_MODE, BUTTON_PIN_POLARITY, BUTTON_DEBOUNCE_PERIOD, BUTTON_LONG_PRESS_PERIOD,
                       BUTTON_DOUBLE_CLICK_PERIOD, BUTTON_DEGLITCH_PERIOD);
}

UserConfirmationInterface::UserConfirmationAction Button::check() {
  UserConfirmationAction action = UserConfirmationAction::None;
  if (!_button)
    return action;
  _button->poll();
  if (_button->singleClick()) {
    action = UserConfirmationAction::SingleClick;
  } else if (_button->doubleClick()) {
    action = UserConfirmationAction::DoubleClick;
  } else if (_button->longPress()) {
    action = UserConfirmationAction::LongClick;
  }
  return action;
}

#endif // NATIVE_TESTING
