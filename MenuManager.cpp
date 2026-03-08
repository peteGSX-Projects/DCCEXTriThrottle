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

char MenuManager::_functionNameBuffers[29][14];
char MenuManager::_consistNameBuffers[10][6];

MenuManager::MenuManager(EventManager *eventManager, Logger *logger)
    : _eventManager(eventManager), _logger(logger), _currentMenu(nullptr), _activeThrottleIndex(-1), _historyIndex(-1),
      _rootMenu(nullptr), _rosterMenu(nullptr), _turnoutMenu(nullptr), _turntableMenu(nullptr), _routeMenu(nullptr),
      _automationMenu(nullptr), _functionMenu(nullptr), _selectConsistMenu(nullptr), _menuCount(0) {}

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
  _selectConsistMenu = _createManagedMenu("Select Consist");

  // Create throttle menus
  Menu *throttle0Menu = _createThrottleMenu(0);
  Menu *throttle1Menu = _createThrottleMenu(1);
  Menu *throttle2Menu = _createThrottleMenu(2);

  // Setup sub menus and items
  _setupTracksMenu(tracksMenu);
  EventData sysInfoData(AppState::DisplaySysInfo, -1);
  ActionMenuItem *sysInfo = new ActionMenuItem("System Info", EventType::RequestStateChange, sysInfoData);

  // Setup main menu items
  _rootMenu->addItem(new ThrottleMenuItem(throttle0Menu, 0)); // 0
  _rootMenu->addItem(new ThrottleMenuItem(throttle1Menu, 1)); // 1
  _rootMenu->addItem(new ThrottleMenuItem(throttle2Menu, 2)); // 2
  _rootMenu->addItem(new SubMenuItem(_turnoutMenu));          // 3
  _rootMenu->addItem(new SubMenuItem(_turntableMenu));        // 4
  _rootMenu->addItem(new SubMenuItem(_routeMenu));            // 5
  _rootMenu->addItem(new SubMenuItem(_rosterMenu));           // 6
  _rootMenu->addItem(new SubMenuItem(tracksMenu));            // 7
  _rootMenu->addItem(sysInfo);                                // 8
}

void MenuManager::handleUserInput(UserInputInterface::UserInputEvent inputEvent) {
  if (!_currentMenu || inputEvent.key == '\0' || inputEvent.action == UserInputInterface::UserInputAction::None)
    return;

  char key = inputEvent.key;
  // LOG(LogLevel::LOG_DEBUG, "MenuManager::handleUserInput() key: ", key);

  if (key >= '0' && key <= '9') {
    _handleSelection(key - '0', inputEvent.action);
  } else if (key == '*' && inputEvent.action == UserInputInterface::UserInputAction::Pressed) {
    _handleBack();
  } else if (key == '#' && inputEvent.action == UserInputInterface::UserInputAction::Pressed) {
    _handleNextPage();
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
    if (loco->getName() == nullptr) {
      LOG(LogLevel::LOG_WARN, "MenuManager::createRosterMenu(): loco has no name, address: ", loco->getAddress());
    } else {
      _rosterMenu->addItem(new LocoMenuItem(loco));
    }
  }
}

void MenuManager::createTurnoutMenu(Turnout *firstTurnout) {
  if (_turnoutMenu == nullptr)
    return;

  // Make sure menu is clear first
  _turnoutMenu->clearItems();

  if (firstTurnout == nullptr)
    return;

  for (Turnout *turnout = firstTurnout; turnout; turnout = turnout->getNext()) {
    if (turnout->getName() == nullptr) {
      LOG(LogLevel::LOG_WARN, "MenuManager::createTurnoutMenu(): turnout has no name, id: ", turnout->getId());
    } else {
      EventType type = EventType::ToggleTurnout;
      EventData data(turnout->getId());
      _turnoutMenu->addItem(new ActionMenuItem(turnout->getName(), type, data));
    }
  }
}

void MenuManager::createRouteMenus(Route *firstRoute) {
  if (_routeMenu == nullptr || _automationMenu == nullptr)
    return;

  // Make sure menus are clear first
  _routeMenu->clearItems();
  _automationMenu->clearItems();

  if (firstRoute == nullptr)
    return;

  for (Route *route = firstRoute; route; route = route->getNext()) {
    if (route->getName() == nullptr) {
      LOG(LogLevel::LOG_WARN, "MenuManager::createRouteMenus(): route has no name, id: ", route->getId());
    } else {
      int routeId = route->getId();
      EventData data(routeId);
      if (route->getType() == RouteType::RouteTypeRoute) {
        EventType type = EventType::StartRoute;
        _routeMenu->addItem(new ActionMenuItem(route->getName(), type, data));
      } else if (route->getType() == RouteType::RouteTypeAutomation) {
        EventType type = EventType::StartAutomation;
        _automationMenu->addItem(new ActionMenuItem(route->getName(), type, data));
      }
    }
  }
}

void MenuManager::createTurntableMenu(Turntable *firstTurntable) {
  if (_turntableMenu == nullptr)
    return;

  // Make sure menus are clear first
  _turntableMenu->clearItems();

  if (firstTurntable == nullptr)
    return;

  for (Turntable *turntable = firstTurntable; turntable; turntable = turntable->getNext()) {
    if (turntable->getName() == nullptr) {
      LOG(LogLevel::LOG_WARN, "MenuManager::createTurntableMenu(): turntable has no name, id: ", turntable->getId());
    } else {
      int ttId = turntable->getId();
      Menu *ttMenu = _createManagedMenu(turntable->getName());
      for (TurntableIndex *ttIndex = turntable->getFirstIndex(); ttIndex; ttIndex = ttIndex->getNextIndex()) {
        if (ttIndex->getName() == nullptr) {
          LOG(LogLevel::LOG_WARN, "MenuManager::createTurntableMenu(): index has no name, id: ", ttIndex->getId());
        } else {
          int indexId = ttIndex->getId();
          EventData rotateData(ttId, indexId);
          ttMenu->addItem(new ActionMenuItem(ttIndex->getName(), EventType::RotateTurntable, rotateData));
        }
      }
      _turntableMenu->addItem(new SubMenuItem(ttMenu));
    }
  }
}

void MenuManager::setupFunctionMenu(Loco *loco, int throttleIndex) {
  if (!loco)
    return;

  if (_functionMenu == nullptr) {
    _functionMenu = _createManagedMenu(loco->getName());
  } else {
    _functionMenu->clearItems();
    _functionMenu->setName(loco->getName());
  }

  _currentMenu = _functionMenu;
  for (int function = 0; function < 29; function++) {
    const char *name = loco->getFunctionName(function);
    bool isMomentary = loco->isFunctionMomentary(function);
    char *functionName = _functionNameBuffers[function];
    int position = 0;

    // Build the name, start with momentary indicator if true
    if (isMomentary)
      functionName[position++] = '*';

    if (name && name[0] != '\0') {
      // If it has a name, copy it, accounting for * and null terminator
      strncpy(&functionName[position], name, 14 - position - 1);
    } else {
      // Otherwise make generic F name
      functionName[position++] = 'F';
      itoa(function, &functionName[position], 10);
    }
    // Ensure null terminator is there
    functionName[sizeof(functionName) - 1] = '\0';

    _functionMenu->addItem(
        new ActionMenuItem(functionName, EventType::ToggleLocoFunction,
                           EventData(function, throttleIndex, UserInputInterface::UserInputAction::None)));
  }
}

void MenuManager::setupConsistMenu(CSConsist *firstConsist, int throttleIndex) {
  if (!firstConsist || firstConsist->getFirstMember() == nullptr)
    return;

  _selectConsistMenu->clearItems();

  int bufferIndex = 0;
  for (CSConsist *consist = firstConsist; consist; consist = consist->getNext()) {
    if (bufferIndex >= 10)
      break;

    Loco *loco = Loco::getByAddress(consist->getFirstMember()->address);
    const char *displayName = nullptr;
    if (loco != nullptr && loco->getName() != nullptr) {
      displayName = loco->getName();
    } else {
      displayName = _consistNameBuffers[bufferIndex];
      itoa(consist->getFirstMember()->address, _consistNameBuffers[bufferIndex], 10);
      bufferIndex++;
    }
    _selectConsistMenu->addItem(
        new ActionMenuItem(displayName, EventType::LocoSelected, EventData(consist, throttleIndex)));
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
    // LOG(LogLevel::LOG_DEBUG, "MenuManager::_handleBack(): Exit Menu");
    reset();
    _eventManager->publish(EventType::ExitMenu, EventData());
  } else {
    // Restore the navigation state
    _currentMenu = node.menu;
    _activeThrottleIndex = node.throttleIndex;
    // LOG(LogLevel::LOG_DEBUG, "MenuManager::_handleBack(): Back to %s", _currentMenu->getName());
    _eventManager->publish(EventType::MenuRefreshRequired, EventData());
  }
}

void MenuManager::_handleNextPage() {
  if (_currentMenu->getTotalPages() > 0) {
    _currentMenu->nextPage();
    _eventManager->publish(EventType::MenuRefreshRequired, EventData());
  }
}

void MenuManager::_handleSelection(int digit, UserInputInterface::UserInputAction action) {
  BaseMenuItem *item = _currentMenu->getItemByPageIndex(digit);

  if (item == nullptr) {
    // LOG(LogLevel::LOG_DEBUG, "MenuManager::_handleSelection(): No item at this index: ", digit);
    return;
  }
  // LOG(LogLevel::LOG_DEBUG, "MenuManager::_handleSelection() select: ", item->getName());

  switch (item->getItemType()) {
  case MenuItemType::ThrottleMenuType: {
    if (action == UserInputInterface::UserInputAction::Pressed) {
      ThrottleMenuItem *throttleMenu = static_cast<ThrottleMenuItem *>(item);

      // Save current navigation state
      _push(_currentMenu, _activeThrottleIndex);

      _activeThrottleIndex = throttleMenu->getThrottleIndex();
      _currentMenu = throttleMenu->getMenu();
      _currentMenu->setCurrentPage(0);
      _eventManager->publish(EventType::MenuRefreshRequired, EventData());
    }
    break;
  }
  case MenuItemType::SubMenuType: {
    if (action == UserInputInterface::UserInputAction::Pressed) {
      SubMenuItem *subMenu = static_cast<SubMenuItem *>(item);

      // Save current navigation state
      _push(_currentMenu, _activeThrottleIndex);

      _currentMenu = subMenu->getMenu();
      _currentMenu->setCurrentPage(0);
      _eventManager->publish(EventType::MenuRefreshRequired, EventData());
    }
    break;
  }
  case MenuItemType::LocoType: {
    if (action == UserInputInterface::UserInputAction::Pressed) {
      if (_activeThrottleIndex != -1) {
        LocoMenuItem *locoItem = static_cast<LocoMenuItem *>(item);
        EventData eventData(locoItem->getLoco(), _activeThrottleIndex);
        _eventManager->publish(EventType::LocoSelected, eventData);
      }
    }
    break;
  }
  case MenuItemType::ActionMenuType: {
    ActionMenuItem *actionItem = static_cast<ActionMenuItem *>(item);

    EventType eventType = actionItem->getEventType();
    EventData eventData = actionItem->getEventData();
    bool publish = false;

    if (eventType == EventType::ToggleLocoFunction) {
      eventData.locoFunctionValue.action = action;
      publish = true;
    } else if (action == UserInputInterface::UserInputAction::Pressed) {
      publish = true;
    }

    if (publish)
      _eventManager->publish(eventType, eventData);
    break;
  }
  default: {
    // LOG(LogLevel::LOG_DEBUG, "MenuManager::_handleSelection(): Unhandled MenuItemType: ", (int)item->getItemType());
    break;
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

Menu *MenuManager::_createManagedMenu(const char *name, int itemsPerPage) {
  if (_menuCount < _MAX_MANAGED_MENUS) {
    Menu *newMenu = new Menu(name, itemsPerPage);
    _allManagedMenus[_menuCount++] = newMenu;
    return newMenu;
  }
  return nullptr;
}

Menu *MenuManager::_createThrottleMenu(int index) {
  const char *name = nullptr;
  if (index == 0) {
    name = "Throttle 1";
  } else if (index == 1) {
    name = "Throttle 2";
  } else if (index == 2) {
    name = "Throttle 3";
  }

  // All throttles need the same Consist menu
  Menu *consistMenu = _createManagedMenu("Consist");

  consistMenu->addItem(new SubMenuItem(_selectConsistMenu, "Select Consist"));
  consistMenu->addItem(new ActionMenuItem("Add from Roster", EventType::ManageConsist, EventData()));
  consistMenu->addItem(new ActionMenuItem("Add from Address", EventType::ManageConsist, EventData()));
  consistMenu->addItem(new ActionMenuItem("Remove Member", EventType::ManageConsist, EventData()));

  Menu *throttleMenu = _createManagedMenu(name);
  // Add Select Loco as the first item to select from roster (0)
  throttleMenu->addItem(new SubMenuItem(_rosterMenu, "Select Loco"));
  // Add Enter Address as an action item to show the user entry screen (1)
  throttleMenu->addItem(
      new ActionMenuItem("Enter Address", EventType::RequestStateChange, EventData(AppState::EnterLocoAddress, index)));
  // Add Automations menu (2)
  throttleMenu->addItem(new SubMenuItem(_automationMenu, "Automations"));
  // Add Consist menu (3)
  throttleMenu->addItem(new SubMenuItem(consistMenu, "Consist"));
  // Add Forget action to clear selection (4)
  throttleMenu->addItem(new ActionMenuItem("Forget", EventType::ForgetLoco, EventData(index)));
  return throttleMenu;
}

void MenuManager::_setupTracksMenu(Menu *tracksMenu) {
  tracksMenu->addItem(new ActionMenuItem("Toggle Power", EventType::ToggleTrackPower, EventData())); // 0
}
