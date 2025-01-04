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

#include "SubMenuItem.h"
#include "Menu.h"

SubMenuItem::SubMenuItem(Menu *menu)
    : BaseMenuItem(menu->getName(), MenuItemType::SubMenuType), _menu(menu) {}

Menu *SubMenuItem::getMenu() { return _menu; }

BaseMenuItem *SubMenuItem::clone() const { return new SubMenuItem(*this); }

SubMenuItem::~SubMenuItem() {
  if (_menu != nullptr) {
    delete _menu;
    _menu = nullptr;
  }
}
