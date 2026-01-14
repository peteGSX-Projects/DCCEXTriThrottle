/*
 *  © 2026 Peter Cole
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

Menu::Menu(const char *name, int itemsPerPage)
    : _firstItem(nullptr), _parent(nullptr), _nextItemIndex(0), _currentPage(0) {
  if (name != nullptr) {
    int nameLength = strlen(name);
    _name = new char[nameLength + 1];
    strcpy(_name, name);
  } else {
    _name = nullptr;
  }
  _itemsPerPage = itemsPerPage;
}

const char *Menu::getName() { return _name; }

void Menu::addItem(BaseMenuItem *item) {
  if (item == nullptr)
    return;

  item->setIndex(_nextItemIndex++);

  // Set parent menu if this is a submenu
  if (item->getItemType() == MenuItemType::SubMenuType) {
    SubMenuItem *subMenu = static_cast<SubMenuItem *>(item);
    Menu *childMenu = subMenu->getMenu();

    if (childMenu != nullptr) {
      childMenu->setParent(this);
    }
  }

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

int Menu::getCurrentPage() { return _currentPage; }

void Menu::setCurrentPage(int page) {
  if (page <= getTotalPages()) {
    _currentPage = page;
  }
}

int Menu::getTotalPages() {
  if (_nextItemIndex == 0)
    return 1;
  return ((_nextItemIndex + _itemsPerPage - 1) / _itemsPerPage);
}

void Menu::nextPage() {
  _currentPage++;
  if (_currentPage >= getTotalPages()) {
    _currentPage = 0;
  }
}

int Menu::getItemCount() { return _nextItemIndex; }

int Menu::getItemsPerPage() { return _itemsPerPage; }

BaseMenuItem *Menu::getItemByPageIndex(int index) {
  // Out of bounds check
  if (index < 0 || index >= _itemsPerPage)
    return nullptr;

  // Map index against current page and items per page
  int menuIndex = (_currentPage * _itemsPerPage) + index;

  // Now find this item in the list
  BaseMenuItem *current = _firstItem;
  while (current != nullptr) {
    if (current->getIndex() == menuIndex) {
      return current;
    }
    current = current->getNext();
  }

  // If we get here, index doesn't exist on this page
  return nullptr;
}

void Menu::clearItems() {
  // Iterate through the list and delete the items
  BaseMenuItem *currentItem = _firstItem;
  while (currentItem != nullptr) {
    BaseMenuItem *nextItem = currentItem->getNext();
    delete currentItem;
    currentItem = nextItem;
  }

  _firstItem = nullptr;
  _nextItemIndex = 0;
  _currentPage = 0;
}

Menu::~Menu() {
  if (_name != nullptr) {
    delete[] _name;
    _name = nullptr;
  }

  clearItems();

  _parent = nullptr;
}
