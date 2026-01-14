/*
 *  © 2026 Peter Cole
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

#include "MenuManager.h"

MenuManager::MenuManager(EventManager *eventManager, Logger *logger)
    : _eventManager(eventManager), _logger(logger), _currentMenu(nullptr), _activeThrottleIndex(-1) {}

void MenuManager::handleUserInput(UserInputInterface::UserInputEvent inputEvent) {
  if (!_currentMenu)
    return;

  if (inputEvent.key != '\0') {
    LOG(LogLevel::LOG_DEBUG, "MenuManager::handleUserInput() key: %c", inputEvent.key);
  }

  switch (inputEvent.key) {
  case '*': {
    _handleBack();
    break;
  }
  case '#': {
    _handleNextPage();
    break;
  }
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9': {
    int digit = inputEvent.key - '0';
    _handleSelection(digit);
    break;
  }
  default: {
    LOG(LogLevel::LOG_DEBUG, "MenuManager::handleUserInput() unknown key %c", inputEvent.key);
    break;
  }
  }
}

Menu *MenuManager::getCurrentMenu() { return _currentMenu; }

void MenuManager::setCurrentMenu(Menu *menu) { _currentMenu = menu; }

bool MenuManager::isAtRootMenu() { return (_currentMenu == nullptr || _currentMenu->getParent() == nullptr); }

int MenuManager::getActiveThrottleIndex() { return _activeThrottleIndex; }

void MenuManager::setActiveThrottleIndex(int index) { _activeThrottleIndex = index; }

MenuManager::~MenuManager() {}

void MenuManager::_handleBack() {
  Menu *parent = _currentMenu->getParent();

  // If we're at the top, exit the menu system and reset throttle context
  if (parent == nullptr) {
    _activeThrottleIndex = -1;
    _eventManager->publish(EventType::ExitMenu, EventData());
  } else {
    _currentMenu = parent;
    // If we're going to the top, reset throttle context
    if (_currentMenu->getParent() == nullptr) {
      _activeThrottleIndex = -1;
    }
    _eventManager->publish(EventType::MenuRefreshRequired, EventData());
  }
}

void MenuManager::_handleNextPage() {
  if (_currentMenu->getTotalPages() > 0) {
    _currentMenu->nextPage();
    _eventManager->publish(EventType::MenuRefreshRequired, EventData());
  }
}

void MenuManager::_handleSelection(int digit) {
  BaseMenuItem *item = _currentMenu->getItemByPageIndex(digit);

  if (item == nullptr) {
    LOG(LogLevel::LOG_DEBUG, "MenuManager::_handleSelection(%d): No item at this index", digit);
    return;
  }

  switch (item->getItemType()) {
  case MenuItemType::ThrottleMenuType: {
    ThrottleMenuItem *throttleMenu = static_cast<ThrottleMenuItem *>(item);
    _activeThrottleIndex = throttleMenu->getThrottleIndex();
    _currentMenu = throttleMenu->getMenu();
    _currentMenu->setCurrentPage(0);
    _eventManager->publish(EventType::MenuRefreshRequired, EventData());
  }
  case MenuItemType::SubMenuType: {
    SubMenuItem *subMenu = static_cast<SubMenuItem *>(item);
    _currentMenu = subMenu->getMenu();
    _currentMenu->setCurrentPage(0);
    _eventManager->publish(EventType::MenuRefreshRequired, EventData());
    break;
  }
  case MenuItemType::LocoType: {
    if (_activeThrottleIndex != -1) {
      LocoMenuItem *locoItem = static_cast<LocoMenuItem *>(item);
      EventData eventData(locoItem->getLoco(), _activeThrottleIndex);
      _eventManager->publish(EventType::LocoSelected, eventData);
    } else {
      LOG(LogLevel::LOG_DEBUG, "MenuManager::_handleSelection(): Loco selected with no throttle context, ignoring");
    }
    break;
  }
  default: {
    LOG(LogLevel::LOG_DEBUG, "MenuManager::_handleSelection(%d): Unhandled MenuItemType %d", digit,
        item->getItemType());
  }
  }
}
