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

#ifndef DISPLAYINTERFACE_H
#define DISPLAYINTERFACE_H

#include "Logger.h"
#include "Menu.h"
#include "Throttle.h"

/**
 * @brief DisplayInterface to abstract physical display objects from display interactions, extend this class when adding
 * implementing physical display objects
 */
class DisplayInterface {
public:
  /**
   * @brief Implement this and call once to perform initialisation or startup methods for a physical display
   */
  virtual void begin() = 0;

  /**
   * @brief Implement this to allow the entire physical display to be cleared
   */
  virtual void clear() = 0;

  /**
   * @brief Display the startup screen
   * @param headerText Text to display in the header
   * @param version Version of the software to be displayed
   */
  virtual void displayStartupScreen(const char *headerText, const char *version) = 0;

  /**
   * @brief Display the throttle screen
   */
  virtual void displayThrottleScreen() = 0;

  /**
   * @brief Update the display for the specified throttle
   * @param throttleIndex Index of the throttle being updated
   * @param throttle Pointer to the Throttle instance
   */
  virtual void updateThrottleScreen(int throttleIndex, Throttle *throttle) = 0;

  /**
   * @brief Update the state of track power on the throttle screen
   * @param state TrackPower
   */
  virtual void updateThrottleTrackPower(TrackPower state) = 0;

  /**
   * @brief Display a menu
   * @param menu Pointer to the menu to display
   */
  virtual void displayMenuScreen(Menu *menu) = 0;

  /**
   * @brief Display the connection error screen
   */
  virtual void displayConnectionErrorScreen() = 0;

  /**
   * @brief Set the redraw flag for the display
   * @param redraw True if redraw is needed, false if not
   */
  virtual void setRedraw(bool redraw) { _needsRedraw = redraw; }

  /**
   * @brief Check if this display needs to be redrawn
   * @return true If needed
   * @return false If not
   */
  virtual bool needsRedraw() { return _needsRedraw; }

  /**
   * @brief Destroy the Display Interface object
   */
  virtual ~DisplayInterface() = default;

protected:
  bool _needsRedraw = true;
};

#endif // DISPLAYINTERFACE_H
