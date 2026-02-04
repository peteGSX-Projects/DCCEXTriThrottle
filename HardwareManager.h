/*
 *  © 2026 Peter Cole
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

#ifndef HARDWAREMANAGER_H
#define HARDWAREMANAGER_H

#include "Defines.h"

class HardwareManager {
public:
  /**
   * @brief Perform hardware setup (eg. disable JTAG on Bluepill)
   */
  static void initialise();

  /**
   * @brief Get the Free Memory object
   * @return int Free RAM in bytes
   */
  static int getFreeMemory();
};

#endif // HARDWAREMANAGER_H
