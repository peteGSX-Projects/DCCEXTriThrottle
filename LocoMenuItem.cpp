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
    // Get the address
    int address = loco->getAddress();
    // Use log10 to get the number of digits, or 1 if it is 0
    int digits = (address == 0) ? 1 : (int)log10(abs(address)) + 1;
    // Add null terminator
    int charSize = digits + 1;
    // Now create the new char array and allocate to _name
    _name = new char[charSize];
    snprintf(_name, charSize, "%d", address);
  }
}

LocoMenuItem::~LocoMenuItem() {
  if (_loco != nullptr) {
    _loco = nullptr;
  }
}
