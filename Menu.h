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

#ifndef MENU_H
#define MENU_H

#include "BaseMenuItem.h"
#include "LocoMenuItem.h"
#include "SubMenuItem.h"

/**
 * @brief Menu class supporting nested menus.
 */
class Menu {
public:
  /// @brief Constructor for this Menu
  /// @param name Name or label for this menu
  Menu(const char *name);

  /// @brief Get the name or label of this Menu
  /// @return Name or label of this Menu
  const char *getName();

  /// @brief Add a menu item to this menu and automatically set its index
  /// @param item Pointer to the menu item to add
  void addItem(BaseMenuItem *item);

  /// @brief Get the first menu item in the list
  /// @return Pointer to the first menu item
  BaseMenuItem *getFirstItem();

  /// @brief Set this Menu's parent if it is nested
  /// @param parent Pointer to the parent Menu
  void setParent(Menu *parent);

  /// @brief Get this Menu's parent if it is nested
  /// @return Pointer to the parent Menu
  Menu *getParent();

  /// @brief Destructor for this Menu
  ~Menu();

private:
  char *_name;              /** Name or label of this menu */
  BaseMenuItem *_firstItem; /** Pointer to the first menu item in the list */
  Menu *_parent;            /** Pointer to a parent menu if this is nested */
  int _nextItemIndex;       /** Auto increment index of next item added */
};

#endif // MENU_H
