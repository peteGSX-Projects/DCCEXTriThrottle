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

#include "BaseMenuItem.h"

BaseMenuItem::BaseMenuItem(const char *name, MenuItemType itemType)
    : _name(name), _itemType(itemType), _index(-1), _next(nullptr) {}

void BaseMenuItem::setIndex(int index) { _index = index; }

int BaseMenuItem::getIndex() { return _index; }

const char *BaseMenuItem::getName() { return _name; }

void BaseMenuItem::setNext(BaseMenuItem *next) { _next = next; }

BaseMenuItem *BaseMenuItem::getNext() { return _next; }

MenuItemType BaseMenuItem::getItemType() { return _itemType; }

BaseMenuItem::~BaseMenuItem() { _next = nullptr; }
