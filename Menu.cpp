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

#include "Menu.h"

Menu::Menu(const char *name) : _firstItem(nullptr), _parent(nullptr), _nextItemIndex(0) {
  if (name != nullptr) {
    int nameLength = strlen(name);
    _name = new char[nameLength + 1];
    strcpy(_name, name);
  } else {
    _name = nullptr;
  }
}

const char *Menu::getName() { return _name; }

void Menu::addItem(BaseMenuItem *item) {
  if (item == nullptr) {
    return;
  }
  item->setIndex(_nextItemIndex++);

  if (_firstItem == nullptr) {
    _firstItem = item;
    return;
  }
  BaseMenuItem *currentItem = _firstItem;
  while (currentItem->getNext() != nullptr) {
    currentItem = currentItem->getNext();
  }
  currentItem->setNext(item);
}

BaseMenuItem *Menu::getFirstItem() { return _firstItem; }

void Menu::setParent(Menu *parent) { _parent = parent; }

Menu *Menu::getParent() { return _parent; }

Menu::~Menu() {
  if (_name != nullptr) {
    delete[] _name;
    _name = nullptr;
  }

  if (_firstItem != nullptr) {
    BaseMenuItem *currentItem = _firstItem;
    while (currentItem != nullptr) {
      BaseMenuItem *nextItem = currentItem->getNext();
      delete currentItem;
      currentItem = nextItem;
    }
    _firstItem = nullptr;
  }

  _parent = nullptr;
}
