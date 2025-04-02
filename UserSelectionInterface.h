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

#ifndef USERSELECTIONINTERFACE_H
#define USERSELECTIONINTERFACE_H

#include "Logger.h"

/**
 * @brief Interface class to abstract user selection actions from physical implementations
 * All physical user selection classes must extend this
 * This allows for the use of physical buttons, rotary encoders, touch screens, and other user interface methods that
 * can move menu selections up and down, and increase or decrease throttle speeds
 */
class UserSelectionInterface {
public:
  /**
   * @brief User selection action data types available
   */
  enum class UserSelectionAction { None, Up, UpFaster, UpFastest, Down, DownFaster, DownFastest };

  /**
   * @brief Implement this to perform any initial startup methods or activities
   */
  virtual void begin() = 0;

  /**
   * @brief Get the User Selection Action object, this should be called at least once per main loop iteration
   * @return UserSelectionAction
   */
  virtual UserSelectionAction check() = 0;

  /**
   * @brief Destroy the User Selection Interface object
   */
  virtual ~UserSelectionInterface() = default;

  /**
   * @brief Check if throttle is inverted compared to up/down menu selection, handy for rotary encoders
   * @return true - UserSelectionAction::Up decreases speed, UserSelectionAction::Down increases speed
   * @return false - Up increases speed, down decreases speed
   */
  bool throttleInverted() { return _throttleInverted; }

  /**
   * @brief Set the Throttle Inverted object
   */
  void setThrottleInverted() { _throttleInverted = true; }

private:
  bool _throttleInverted = false;
};

#endif // USERSELECTIONINTERFACE_H
