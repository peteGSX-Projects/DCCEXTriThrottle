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

#ifndef BUTTON_H
#define BUTTON_H

// Do not use this for testing
#ifndef NATIVE_TESTING

#include "UserConfirmationInterface.h"
#include "avdweb_Switch.h"
#include <Arduino.h>

/**
 * @brief Define defaults for Switch objects
 */
#ifndef BUTTON_PIN_MODE
#define BUTTON_PIN_MODE INPUT_PULLUP
#endif // BUTTON_PIN_MODE
#ifndef BUTTON_PIN_POLARITY
#define BUTTON_PIN_POLARITY LOW
#endif // BUTTON_PIN_POLARITY
#ifndef BUTTON_DEBOUNCE_PERIOD
#define BUTTON_DEBOUNCE_PERIOD 50
#endif // BUTTON_DEBOUNCE_PERIOD
#ifndef BUTTON_LONG_PRESS_PERIOD
#define BUTTON_LONG_PRESS_PERIOD 500
#endif // BUTTON_LONG_PRESS_PERIOD
#ifndef BUTTON_DOUBLE_CLICK_PERIOD
#define BUTTON_DOUBLE_CLICK_PERIOD 250
#endif // BUTTON_DOUBLE_CLICK_PERIOD
#ifndef BUTTON_DEGLITCH_PERIOD
#define BUTTON_DEGLITCH_PERIOD 10
#endif // BUTTON_DEGLITCH_PERIOD

/**
 * @brief This Button class implements the UserConfirmationInterface using an avdweb_Switch Switch object for user
 * interaction.
 */
class Button : public UserConfirmationInterface {
public:
  /**
   * @brief Construct a new Button object
   * @param pin Pin the button is connected to
   */
  Button(byte pin);

  /**
   * @brief Initialise the button pin - doesn't do anything for Switch objects
   */
  void begin() override {}

  /**
   * @brief Call check() frequently to check for button events
   * @return UserConfirmationAction
   */
  UserConfirmationAction check() override;

private:
  Switch *_button;
};

#endif // NATIVE_TESTING

#endif // BUTTON_H
