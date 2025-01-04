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

#ifndef SUBMENUITEM_H
#define SUBMENUITEM_H

#include "BaseMenuItem.h"

class Menu;

/// @brief This menu item type allows for a submenu to allow nested menus
class SubMenuItem : public BaseMenuItem {
public:
  /// @brief Constructor for this SubMenuItem
  /// @param menu Pointer to the Menu instance associated with this item
  SubMenuItem(Menu *menu);

  /// @brief Get the Menu instance associated with this item
  /// @return Pointer to the Menu instance
  Menu *getMenu();

  /// @brief Override to return the pointer to this instance for polymorphic clean up
  /// @return Pointer to this instance as a BaseMenuItem
  BaseMenuItem *clone() const override;

  /// @brief Destructor for this SubMenuItem
  ~SubMenuItem();
private:
  Menu *_menu;
};

#endif // SUBMENUITEM_H
