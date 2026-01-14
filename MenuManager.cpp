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
    : _eventManager(eventManager), _logger(logger), _currentMenu(nullptr) {}

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

MenuManager::~MenuManager() {}

void MenuManager::_handleBack() {
  Menu *parent = _currentMenu->getParent();

  // If we're at the top, exit the menu system
  if (parent == nullptr) {
    _eventManager->publish(EventType::ExitMenu, EventData());
  } else {
    _currentMenu = parent;
    _eventManager->publish(EventType::MenuRefreshRequired, EventData());
  }
}

void MenuManager::_handleNextPage() {
  if (_currentMenu->getTotalPages() > 0) {
    _currentMenu->nextPage();
    _eventManager->publish(EventType::MenuRefreshRequired, EventData());
  }
}

void MenuManager::_handleSelection(int digit) {}
