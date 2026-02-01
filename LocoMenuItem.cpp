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

LocoMenuItem::LocoMenuItem(Loco *loco) : BaseMenuItem(loco->getName(), MenuItemType::LocoType, false), _loco(loco) {
  // If there's no name for this loco, create one from the address
  if (loco->getName() == nullptr) {
    char buffer[6];
    itoa(loco->getAddress(), buffer, 10);
    int len = strlen(buffer);
    char *nameCopy = new char[len + 1];
    strcpy(nameCopy, buffer);
    _name = nameCopy;
    // This allocated name will be deleted in destructor
    _isProgmem = false;
  }
}

Loco *LocoMenuItem::getLoco() { return _loco; }

LocoMenuItem::~LocoMenuItem() {
  // Clean up dynamically allocated name if it was created from address
  if (_name != nullptr && !_isProgmem) {
    // Only delete if it's a SRAM-allocated copy (created from address above)
    if (_loco != nullptr && _name != _loco->getName()) {
      delete[] const_cast<char*>(_name);
    }
  }
  if (_loco != nullptr) {
    _loco = nullptr;
  }
}
