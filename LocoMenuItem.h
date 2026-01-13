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

#ifndef LOCOMENUITEM_H
#define LOCOMENUITEM_H

#include "BaseMenuItem.h"
#include <DCCEXProtocol.h>

class LocoMenuItem : public BaseMenuItem {
public:
  /// @brief Constructor for this LocoMenuItem
  /// @param loco Pointer to the associated Loco instance
  LocoMenuItem(Loco *loco);

  /// @brief Destructor for this LocoMenuItem
  ~LocoMenuItem();

private:
  Loco *_loco; /** Loco instance associated with this item */
};

#endif // LOCOMENUITEM_H
