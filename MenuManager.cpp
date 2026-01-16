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
    : _eventManager(eventManager), _logger(logger), _currentMenu(nullptr), _activeThrottleIndex(-1), _historyIndex(-1),
      _rootMenu(nullptr), _rosterMenu(nullptr), _turnoutMenu(nullptr), _turntableMenu(nullptr), _routeMenu(nullptr),
      _automationMenu(nullptr), _menuCount(0) {}

void MenuManager::initialise() {
  // Create Menu instances
  _rootMenu = _createManagedMenu("Main Menu");
  _currentMenu = _rootMenu;
  _rosterMenu = _createManagedMenu("Roster");
  _turnoutMenu = _createManagedMenu("Turnouts");
  _turntableMenu = _createManagedMenu("Turntables");
  _routeMenu = _createManagedMenu("Routes");
  _automationMenu = _createManagedMenu("Automations");
  Menu *tracksMenu = _createManagedMenu("Tracks");
  Menu *systemMenu = _createManagedMenu("System");

  // Create throttle menus
  Menu *throttle0Menu = _createThrottleMenu(0);
  Menu *throttle1Menu = _createThrottleMenu(1);
  Menu *throttle2Menu = _createThrottleMenu(2);

  // Setup main menu items
  _rootMenu->addItem(new ThrottleMenuItem(throttle0Menu, 0));
  _rootMenu->addItem(new ThrottleMenuItem(throttle1Menu, 1));
  _rootMenu->addItem(new ThrottleMenuItem(throttle2Menu, 2));
  _rootMenu->addItem(new SubMenuItem(_turnoutMenu));
  _rootMenu->addItem(new SubMenuItem(_turntableMenu));
  _rootMenu->addItem(new SubMenuItem(_routeMenu));
  _rootMenu->addItem(new SubMenuItem(_rosterMenu));
  _rootMenu->addItem(new SubMenuItem(tracksMenu));
  _rootMenu->addItem(new SubMenuItem(systemMenu));
}

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

bool MenuManager::isAtRootMenu() { return (_historyIndex == -1); }

int MenuManager::getActiveThrottleIndex() { return _activeThrottleIndex; }

void MenuManager::setActiveThrottleIndex(int index) { _activeThrottleIndex = index; }

void MenuManager::reset() {
  _historyIndex = -1;
  _activeThrottleIndex = -1;
  _currentMenu = _rootMenu;
}

void MenuManager::setRootMenu(Menu *menu) { _rootMenu = menu; }

Menu *MenuManager::getRootMenu() { return _rootMenu; }

void MenuManager::createRosterMenu(Loco *roster) {
  if (_rosterMenu == nullptr)
    return;

  // Make sure the menu is clear before adding new ones
  _rosterMenu->clearItems();

  if (roster == nullptr)
    return;

  for (Loco *loco = roster; loco; loco = loco->getNext()) {
    _rosterMenu->addItem(new LocoMenuItem(loco));
  }
}

MenuManager::~MenuManager() {
  for (int i = 0; i < _menuCount; i++) {
    delete _allManagedMenus[i];
  }
}

void MenuManager::_handleBack() {
  // Get the latest navigation state
  NavigationNode node = _pop();

  // If we're at the top, exit the menu system and reset throttle context
  if (node.menu == nullptr) {
    reset();
    _eventManager->publish(EventType::ExitMenu, EventData());
  } else {
    // Restore the navigation state
    _currentMenu = node.menu;
    _activeThrottleIndex = node.throttleIndex;
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

    // Save current navigation state
    _push(_currentMenu, _activeThrottleIndex);

    _activeThrottleIndex = throttleMenu->getThrottleIndex();
    _currentMenu = throttleMenu->getMenu();
    _currentMenu->setCurrentPage(0);
    _eventManager->publish(EventType::MenuRefreshRequired, EventData());
    break;
  }
  case MenuItemType::SubMenuType: {
    SubMenuItem *subMenu = static_cast<SubMenuItem *>(item);

    // Save current navigation state
    _push(_currentMenu, _activeThrottleIndex);

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

void MenuManager::_push(Menu *menu, int index) {
  if (_historyIndex < _MAX_MENU_DEPTH - 1) {
    _historyIndex++;
    _history[_historyIndex] = {menu, index};
  }
}

MenuManager::NavigationNode MenuManager::_pop() {
  if (_historyIndex >= 0) {
    return _history[_historyIndex--];
  }

  return {nullptr, -1};
}

Menu *MenuManager::_createManagedMenu(const char *name) {
  if (_menuCount < _MAX_MANAGED_MENUS) {
    Menu *newMenu = new Menu(name);
    _allManagedMenus[_menuCount++] = newMenu;
    return newMenu;
  }
  return nullptr;
}

Menu *MenuManager::_createThrottleMenu(int index) {
  char nameBuffer[25];
  int throttleNumber = index + 1;
  snprintf(nameBuffer, sizeof(nameBuffer), "Throttle %d", throttleNumber);
  Menu *throttleMenu = _createManagedMenu(nameBuffer);
  throttleMenu->addItem(new SubMenuItem(_rosterMenu, "Select Loco"));
  return throttleMenu;
}
