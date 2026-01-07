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

#ifndef USERINPUTINTERFACE_H
#define USERINPUTINTERFACE_H

#include "Logger.h"

/**
 * @brief Interface class to abstract key input actions from physical implementations
 * All physical user input classes must extend this
 * This allows for the use of different keypads including 3x4 and 4x4
 */
class UserInputInterface {
public:
  /**
   * @brief User confirmation action data types available
   */
  enum class UserInputAction { None, Pressed, Held, Released };

  /**
   * @brief Structure for a UserInputEvent containing the key pressed and type of press
   */
  struct UserInputEvent {
    char key;
    UserInputAction action;
  };

  /**
   * @brief Implement this to perform any initial startup methods or activities
   */
  virtual void begin() = 0;

  /**
   * @brief Check for user input, should be called at least once per main loop iteration
   * @return UserInputEvent
   */
  virtual UserInputEvent check() = 0;

  /**
   * @brief Destroy the User Input Interface object
   */
  virtual ~UserInputInterface() = default;
};

#endif // USERINPUTINTERFACE_H
