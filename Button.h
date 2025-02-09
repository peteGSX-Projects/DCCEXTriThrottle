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
#ifndef PIO_UNIT_TESTING

#include <Arduino.h>

/**
 * @brief Define default debounce, double press, and hold times
 */
#ifndef BUTTON_DEBOUNCE_TIME
#define BUTTON_DEBOUNCE_TIME 20
#endif // BUTTON_DEBOUNCE_TIME
#ifndef BUTTON_DOUBLE_CLICK_TIME
#define BUTTON_DOUBLE_CLICK_TIME 200
#endif // BUTTON_DOUBLE_CLICK_TIME
#ifndef BUTTON_LONG_CLICK_TIME
#define BUTTON_LONG_CLICK_TIME 500
#endif // BUTTON_LONG_CLICK_TIME

class Button {
public:
  /**
   * @brief Event types for button clicks
   */
  enum class EventType { None, SingleClick, DoubleClick, LongClick, Held };

  /**
   * @brief Construct a new Button object
   * @param pin Pin the button is connected to
   * @param debounceTime Debounce time in milliseconds, adjust to suit the physical button (default 20ms)
   * @param doubleClickTime Time in ms in which a double click must occur (default 200ms)
   * @param longClickTime Time in ms the button must be held to flag a long click (default 500ms)
   */
  Button(byte pin, unsigned long debounceTime = BUTTON_DEBOUNCE_TIME,
         unsigned long doubleClickTime = BUTTON_DOUBLE_CLICK_TIME,
         unsigned long longClickTime = BUTTON_LONG_CLICK_TIME);

  /**
   * @brief Initialise the button pin
   */
  void begin();

  /**
   * @brief Call checkButton() frequently to check for button events
   * @return EventType Returns the EventType
   */
  EventType checkButton();

private:
  // Const private attributes
  const byte _pin;                     /** Pin the button is connected to */
  const unsigned long _debounceTime;   /** Debounce delay in ms */
  const unsigned long _doubleClickTime; /** Time in ms in which a double click must occur */
  const unsigned long _longClickTime;  /** Time in ms to hold to record a long click */

  // Variable private attributes
  unsigned long _lastClickTime;  /** Time of last click for calculations */
  unsigned long _clickStartTime; /** Time click started for calculations */
  uint8_t _clickCount;           /** Number of times clicked for calculations */
  bool _isClicked;               /** Track if clicked for calculations */
  bool _longClickActivated;      /** Track to prevent repetitive click events */

  /**
   * @brief Update the current button state including debouncing and click count
   */
  void _updateButtonState();
};

#endif // PIO_UNIT_TESTING

#endif // BUTTON_H
