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

#ifndef THROTTLE_H
#define THROTTLE_H

#include "UserConfirmationInterface.h"
#include "UserInputInterface.h"
#include "UserSelectionInterface.h"
#include <DCCEXProtocol.h>

/**
 * @brief This class enables interactive control of the selected Loco both when the ThrottleScreen is active, and when
 * the user is interacting with menus or other functions
 */
class Throttle {
public:
  /**
   * @brief Construct a new Throttle object
   * @param confirmer Pointer to a class instance extending the UserConfirmationInterface class
   * @param selector Pointer to a class instance extending the UserSelectionInterface class
   */
  Throttle(UserConfirmationInterface *confirmer, UserSelectionInterface *selector);

private:
  UserConfirmationInterface *_confirmer;
  UserSelectionInterface *_selector;
  Loco *_loco;
};

#endif // THROTTLE_H
