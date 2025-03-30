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

protected:
  bool _needsRedraw = true;
};

#endif // DISPLAYINTERFACE_H
