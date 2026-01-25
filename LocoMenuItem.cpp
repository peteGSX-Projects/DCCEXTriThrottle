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

#include "LocoMenuItem.h"

LocoMenuItem::LocoMenuItem(Loco *loco) : BaseMenuItem(loco->getName(), MenuItemType::LocoType), _loco(loco) {
  // If there's no name for this loco, set the address as the name
  if (loco->getName() == nullptr) {
    if (_name != nullptr) {
      delete[] _name;
    }

    char buffer[6];
    itoa(loco->getAddress(), buffer, 10);
    int len = strlen(buffer);
    _name = new char[len + 1];
    strcpy(_name, buffer);
  }
}

Loco *LocoMenuItem::getLoco() { return _loco; }

LocoMenuItem::~LocoMenuItem() {
  if (_loco != nullptr) {
    _loco = nullptr;
  }
}
