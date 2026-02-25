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

#include "AppOrchestrator.h"
#include "HardwareManager.h"
#include "Version.h"

AppOrchestrator::AppOrchestrator(DisplayInterface *displayInterface, UserInputInterface *userInputInterface,
                                 Logger *logger, int numThrottles, Throttle **throttles,
                                 ConnectionManager *connectionManager, EventManager *eventManager,
                                 MenuManager *menuManager, DCCEXProtocol *commandStationClient)
    : _displayInterface(displayInterface), _userInputInterface(userInputInterface), _logger(logger),
      _numThrottles(numThrottles), _throttles(throttles), _connectionManager(connectionManager),
      _eventManager(eventManager), _menuManager(menuManager), _commandStationClient(commandStationClient) {
  // LOG(LogLevel::LOG_DEBUG, "AppOrchestrator() created");
  _currentAppState = AppState::Startup;
  _activeContextIndex = -1;
  _enterAddressBuffer = 0;
  _enterAddressBufferCount = 0;
  _powerState = TrackPower::PowerUnknown;
}

void AppOrchestrator::begin() {
  // LOG(LogLevel::LOG_DEBUG, "AppOrchestrator::begin()");

  // Auto subscribe to events first
  for (int i = 0; i < EventType::EVENT_TYPE_COUNT; i++) {
    _eventManager->subscribe(this, static_cast<EventType>(i));
  }

  if (_displayInterface) {
    _displayInterface->begin();
  }

  if (_userInputInterface) {
    _userInputInterface->begin();
  }

  if (_menuManager) {
    _menuManager->initialise();
  }

  if (_connectionManager)
    _connectionManager->begin();
}

void AppOrchestrator::update() {
  UserInputInterface::UserInputEvent inputEvent = _userInputInterface->check();
  _connectionManager->update();
  // Always handle throttle controls
  for (int i = 0; i < _numThrottles; i++) {
    _throttles[i]->update();
  }
  switch (_currentAppState) {
  case AppState::Startup: {
    _handleStartupState();
    break;
  }
  case AppState::Throttle: {
    _handleThrottleState(inputEvent);
    break;
  }
  case AppState::ConnectionError: {
    _handleConnectionError(inputEvent);
    break;
  }
  case AppState::Menu: {
    _handleMenuState(inputEvent);
    break;
  }
  case AppState::EnterLocoAddress: {
    _handleEnterLocoAddress(inputEvent);
    break;
  }
  case AppState::DisplaySysInfo: {
    _handleDisplaySysInfo(inputEvent);
    break;
  }
  case AppState::OutOfMemory: {
    _handleOutOfMemory();
    break;
  }
  default: {
    LOG(LogLevel::LOG_ERROR, "AppOrchestrator::update(): Unknown AppState: ", (int)_currentAppState);
    break;
  }
  }

  if (_displayInterface->needsRedraw()) {
    _displayCurrentState();
    _displayInterface->setRedraw(false);
  } else if (_currentAppState == AppState::Throttle) {
    _updateThrottleDisplay();
  }
}

void AppOrchestrator::onEvent(Event &event) {
  // LOG(LogLevel::LOG_DEBUG, "AppOrchestrator::onEvent(): ", (int)event.eventType);

  /**
   * @brief Define the function pointer table for all event handlers using the typedef signature
   * @details Every slot must have a valid pointer, so this must be kept in sync with the EventType enum in
   * EventStructure.h
   */
  static const EventHandler eventHandlers[] = {
      &AppOrchestrator::_handleCommandStationConnected, // 0
      &AppOrchestrator::_handleToggleTurnout,           // 1
      &AppOrchestrator::_handleLocoSelected,            // 2
      &AppOrchestrator::_handleReceivedTrackPower,      // 3
      &AppOrchestrator::_handleReceivedReadLoco,        // 4
      &AppOrchestrator::_handleToggleTrackPower,        // 5
      &AppOrchestrator::_handleConnectionRetry,         // 6
      &AppOrchestrator::_handleReadLocoRetry,           // 7
      &AppOrchestrator::_handleExitMenu,                // 8
      &AppOrchestrator::_handleMenuRefreshRequired,     // 9
      &AppOrchestrator::_handleLocoAddressEntered,      // 10
      &AppOrchestrator::_handleRequestStateChange,      // 11
      &AppOrchestrator::_handleStartRoute,              // 12
      &AppOrchestrator::_handleStartAutomation,         // 13
      &AppOrchestrator::_handleRotateTurntable,         // 14
      &AppOrchestrator::_handleForgetLoco               // 15
  };

  // // Set the type index
  int typeIndex = (int)event.eventType;
  // // Calculate table size for the bounds check
  const int tableSize = sizeof(eventHandlers) / sizeof(EventHandler);

  // // Make sure the event type is within the bounds
  if (typeIndex >= 0 && typeIndex < tableSize) {
    // Call the event handler
    EventHandler handler = eventHandlers[typeIndex];
    if (handler != nullptr) {
      (this->*handler)(event);
    }
  } else {
    LOG(LogLevel::LOG_ERROR, "AppOrchestrator::onEvent() unknown event: ", typeIndex);
  }
}

AppState AppOrchestrator::getCurrentAppState() { return _currentAppState; }

void AppOrchestrator::setCurrentAppState(AppState state) { _currentAppState = state; }

int AppOrchestrator::getActiveContextIndex() { return _activeContextIndex; }

void AppOrchestrator::setActiveContextIndex(int index) { _activeContextIndex = index; }

TrackPower AppOrchestrator::getTrackPowerState() { return _powerState; }

AppOrchestrator::~AppOrchestrator() {}

// update() handlers

void AppOrchestrator::_handleStartupState() {
  ConnectionState state = _connectionManager->getState();
  if (state == ConnectionState::Connected) {
    _switchState(AppState::Throttle);
  } else if (state == ConnectionState::Failed) {
    _switchState(AppState::ConnectionError);
  }
}

void AppOrchestrator::_handleThrottleState(UserInputInterface::UserInputEvent event) {
  char key = event.key;
  switch (key) {
  case '*': {
    _switchState(AppState::Menu);
    break;
  }
  case '0': {
    if (event.action == UserInputInterface::UserInputAction::Held) {
      _commandStationClient->emergencyStop();
    } else if (event.action == UserInputInterface::UserInputAction::Pressed) {
      // Track power here
    }
    break;
  }
  case '1':
  case '2':
  case '3': {
    int throttleIndex = key - '1';
    if (event.action == UserInputInterface::UserInputAction::Pressed) {
      if (_throttles[throttleIndex]->getLoco() != nullptr) {
        Loco *loco = _throttles[throttleIndex]->getLoco();
        if (_commandStationClient->isFunctionOn(loco, 0)) {
          _commandStationClient->functionOff(loco, 0);
        } else {
          _commandStationClient->functionOn(loco, 0);
        }
      } else if (_throttles[throttleIndex]->getConsist() != nullptr) {
        Consist *consist = _throttles[throttleIndex]->getConsist();
        if (_commandStationClient->isFunctionOn(consist, 0)) {
          _commandStationClient->functionOff(consist, 0);
        } else {
          _commandStationClient->functionOn(consist, 0);
        }
      }
    }
    break;
  }
  default: {
    break;
  }
  }
}

void AppOrchestrator::_handleConnectionError(UserInputInterface::UserInputEvent event) {
  if (event.key == '*') {
    LOG(LogLevel::LOG_DEBUG, "Entering demo mode");
    _switchState(AppState::Throttle);
  } else if (event.key != '\0') {
    _connectionManager->begin();
    _switchState(AppState::Startup);
  }
}

void AppOrchestrator::_handleMenuState(UserInputInterface::UserInputEvent event) {
  _menuManager->handleUserInput(event);
}

void AppOrchestrator::_handleEnterLocoAddress(UserInputInterface::UserInputEvent event) {
  if (event.action != UserInputInterface::UserInputAction::Pressed)
    return;

  char key = event.key;

  if (key >= '0' && key <= '9') {
    if (_enterAddressBufferCount < 5) {
      _enterAddressBuffer = (_enterAddressBuffer * 10) + (key - '0');
      _enterAddressBufferCount++;
      _displayInterface->displayUserEntryKey(key, _enterAddressBufferCount);
    }
  } else if (key == '*') {
    _enterAddressBuffer = 0;
    _enterAddressBufferCount = 0;
    _switchState(AppState::Menu);
  } else if (key == '#') {
    if (_enterAddressBufferCount > 0) {
      EventData eventData(_enterAddressBuffer, _activeContextIndex);
      Event event(EventType::LocoAddressEntered, eventData);
      _enterAddressBuffer = 0;
      _enterAddressBufferCount = 0;
      _handleLocoAddressEntered(event);
    }
  }
}

void AppOrchestrator::_handleDisplaySysInfo(UserInputInterface::UserInputEvent event) {
  if (event.action != UserInputInterface::UserInputAction::Pressed)
    return;

  if (event.key == '*') {
    _switchState(AppState::Menu);
  }
}

void AppOrchestrator::_handleOutOfMemory() {}

// onEvent() handlers

void AppOrchestrator::_handleCommandStationConnected(Event &event) {
  if (_commandStationClient != nullptr) {
    // On connection, explicitly request server version to update power state and set version variables
    _commandStationClient->requestServerVersion();
    // Setup the roster menu
    Loco *roster = _commandStationClient->roster->getFirst();
    if (roster != nullptr) {
      // LOG(LogLevel::LOG_DEBUG, "AppOrchestrator: _menuManager->createRosterMenu(): ", roster->getName());
      if (HardwareManager::isMemorySafe()) {
        _menuManager->createRosterMenu(roster);
      } else {
        _switchState(AppState::OutOfMemory);
      }
    }
    // Setup the turnout menu
    Turnout *turnout = _commandStationClient->turnouts->getFirst();
    if (turnout != nullptr) {
      // LOG(LogLevel::LOG_DEBUG, "AppOrchestrator: _menuManager->createTurnoutMenu(): ", turnout->getName());
      if (HardwareManager::isMemorySafe()) {
        _menuManager->createTurnoutMenu(turnout);
      } else {
        _switchState(AppState::OutOfMemory);
      }
    }
    // Setup the route and automation menus
    Route *route = _commandStationClient->routes->getFirst();
    if (route != nullptr) {
      // LOG(LogLevel::LOG_DEBUG, "AppOrchestrator: _menuManager->createRouteMenus(): ", route->getName());
      if (HardwareManager::isMemorySafe()) {
        _menuManager->createRouteMenus(route);
      } else {
        _switchState(AppState::OutOfMemory);
      }
    }
    // Setup the turntable menu
    Turntable *turntable = _commandStationClient->turntables->getFirst();
    if (turntable != nullptr) {
      // LOG(LogLevel::LOG_DEBUG, "AppOrchestrator: _menuManager->createTurntableMenu(): ", turntable->getName());
      if (HardwareManager::isMemorySafe()) {
        _menuManager->createTurntableMenu(turntable);
      } else {
        _switchState(AppState::OutOfMemory);
      }
    }
  }
}

void AppOrchestrator::_handleToggleTurnout(Event &event) {
  int turnoutId = event.eventData.intValue;
  _commandStationClient->toggleTurnout(turnoutId);
}

void AppOrchestrator::_handleLocoSelected(Event &event) {
  if (!_throttles)
    return;

  int throttleIndex = event.eventData.selectLocoValue.throttleIndex;
  if (throttleIndex < 0 || throttleIndex >= _numThrottles)
    return;

  Loco *newLoco = event.eventData.selectLocoValue.loco;

  // If throttle has an existing Loco or Consist at speed > 0, ignore
  if (_throttles[throttleIndex]->getLoco() != nullptr || _throttles[throttleIndex]->getConsist() != nullptr) {
    int speed = _throttles[throttleIndex]->getLoco() ? _throttles[throttleIndex]->getLoco()->getSpeed()
                                                     : _throttles[throttleIndex]->getConsist()->getSpeed();
    if (speed > 0)
      return;
  }

  // If this loco address is associated with any other throttle, do nothing
  if (_isLocoAddressAssociated(newLoco->getAddress()))
    return;

  // If the current loco is entered manually, delete it first
  Loco *currentLoco = _throttles[throttleIndex]->getLoco();
  if (currentLoco != nullptr && currentLoco->getSource() == LocoSource::LocoSourceEntry) {
    delete currentLoco;
  }
  _throttles[throttleIndex]->setLoco(newLoco);
  _switchState(AppState::Throttle);
}

void AppOrchestrator::_handleReceivedTrackPower(Event &event) {
  // Always keep track power state current
  TrackPower powerState = event.eventData.trackPowerValue;
  _powerState = powerState;

  if (_currentAppState != AppState::Throttle)
    return;
  // Only update display if in Throttle state
  _displayInterface->updateThrottleTrackPower(powerState);
}

void AppOrchestrator::_handleReceivedReadLoco(Event &event) {}

void AppOrchestrator::_handleToggleTrackPower(Event &event) {
  if (_powerState == TrackPower::PowerOn) {
    _commandStationClient->powerOff();
  } else {
    _commandStationClient->powerOn();
  }
  _menuManager->reset();
  _switchState(AppState::Throttle);
}

void AppOrchestrator::_handleConnectionRetry(Event &event) {
  if (_currentAppState == AppState::Startup) {
    _displayInterface->updateProgressScreen();
  }
}

void AppOrchestrator::_handleReadLocoRetry(Event &event) {}

void AppOrchestrator::_handleExitMenu(Event &event) { _switchState(AppState::Throttle); }

void AppOrchestrator::_handleMenuRefreshRequired(Event &event) { _displayInterface->setRedraw(true); }

void AppOrchestrator::_handleLocoAddressEntered(Event &event) {
  if (!_throttles)
    return;

  int address = event.eventData.locoAddressValue.address;
  int throttleIndex = event.eventData.locoAddressValue.throttleIndex;

  // If address already in use, bail out
  if (_isLocoAddressAssociated(address))
    return;

  // Validate throttle index is within bounds
  if (throttleIndex < 0 || throttleIndex >= _numThrottles)
    return;

  // If throttle has an existing Loco or Consist at speed > 0, ignore
  if (_throttles[throttleIndex]->getLoco() != nullptr || _throttles[throttleIndex]->getConsist() != nullptr) {
    int speed = _throttles[throttleIndex]->getLoco() ? _throttles[throttleIndex]->getLoco()->getSpeed()
                                                     : _throttles[throttleIndex]->getConsist()->getSpeed();
    if (speed > 0)
      return;
  }

  // Need to validate DCC address first, redirect with an error if invalid
  if (address < 1 || address > 10239) {
    LOG(LogLevel::LOG_WARN, "AppOrchestrator:: Invalid DCC address entered: ", address);
    _switchState(AppState::EnterLocoAddress);
    _displayInterface->displayUserEntryScreen("Enter Address", "Invalid address! Retry:");
  } else if (!HardwareManager::isMemorySafe()) {
    _switchState(AppState::OutOfMemory);
  } else {
    // Attempt to retrieve the loco from the roster first
    Loco *loco = _commandStationClient->roster->getByAddress(address);
    if (loco == nullptr) {
      // If there isn't one, create the new loco with the address as the name and associate it
      loco = new Loco(address, LocoSource::LocoSourceEntry);
      char name[6];
      itoa(address, name, 10);
      loco->setName(name);
    }
    Event selectEvent(EventType::LocoSelected, EventData(loco, throttleIndex));
    _handleLocoSelected(selectEvent);
  }
}

void AppOrchestrator::_handleRequestStateChange(Event &event) {
  AppState newState = event.eventData.stateRequestValue.state;
  _activeContextIndex = event.eventData.stateRequestValue.contextIndex;
  _switchState(newState);
}

void AppOrchestrator::_handleStartRoute(Event &event) {
  int routeId = event.eventData.intValue;
  _commandStationClient->startRoute(routeId);
}

void AppOrchestrator::_handleStartAutomation(Event &event) {
  int automationId = event.eventData.locoAddressValue.address;
  int throttleIndex = _menuManager->getActiveThrottleIndex();
  if (!_throttles || throttleIndex < 0 || throttleIndex >= _numThrottles) {
    return;
  }
  // Must have a Loco, and also never send a Consist
  if (_throttles[throttleIndex]->getLoco() != nullptr) {
    int address = _throttles[throttleIndex]->getLoco()->getAddress();
    _commandStationClient->handOffLoco(address, automationId);
    _switchState(AppState::Throttle);
  }
}

void AppOrchestrator::_handleRotateTurntable(Event &event) {
  int turntableId = event.eventData.locoAddressValue.address;
  int indexId = event.eventData.locoAddressValue.throttleIndex;
  _commandStationClient->rotateTurntable(turntableId, indexId);
}

void AppOrchestrator::_handleForgetLoco(Event &event) {
  int throttleIndex = event.eventData.intValue;
  if (!_throttles || throttleIndex < 0 || throttleIndex > 2)
    return;

  _throttles[throttleIndex]->forgetLoco();

  _switchState(AppState::Throttle);
}

// General helper methods

void AppOrchestrator::_switchState(AppState newState) {
  if (newState >= AppState::APP_STATE_COUNT)
    return;

  if (_currentAppState == newState)
    return;

  // LOG(LogLevel::LOG_DEBUG, "AppOrchestrator::_switchState(): ", (int)newState);
  if (newState == AppState::Throttle) {
    _menuManager->reset();
  }
  _displayInterface->setRedraw(true);
  _currentAppState = newState;
}

void AppOrchestrator::_displayCurrentState() {
  switch (_currentAppState) {
  case AppState::Startup: {
    _displayInterface->displayProgressScreen("DCC-EX Tri-Throttle " VERSION, "Connecting...");
    break;
  }
  case AppState::Throttle: {
    _displayInterface->displayThrottleScreen(_throttles, _powerState);
    break;
  }
  case AppState::ConnectionError: {
    _displayInterface->displayErrorScreen("Connection Error", "Could not connect, '*' for demo", false);
    break;
  }
  case AppState::Menu: {
    _displayInterface->displayMenuScreen(_menuManager->getCurrentMenu());
    break;
  }
  case AppState::EnterLocoAddress: {
    char title[] = "Throttle X Address";              // X placeholder
    title[9] = (char)(_activeContextIndex + 1 + '0'); // Overwrite placeholder with throttle number char
    _displayInterface->displayUserEntryScreen(title, "Enter DCC address:");
    break;
  }
  case AppState::DisplaySysInfo: {
    const char *version = VERSION;
    const char *libVersion = _commandStationClient->getLibraryVersion();
    int major = _commandStationClient->getMajorVersion();
    int minor = _commandStationClient->getMinorVersion();
    int patch = _commandStationClient->getPatchVersion();
    int freeBytes = HardwareManager::getFreeMemory();
    _displayInterface->displaySysInfoScreen(version, libVersion, major, minor, patch, freeBytes);
    break;
  }
  case AppState::OutOfMemory: {
    _displayInterface->displayErrorScreen("Memory Error", "Out of memory, system halted", true);
    break;
  }
  default:
    break;
  }
}

void AppOrchestrator::_updateThrottleDisplay() {
  for (int i = 0; i < _numThrottles; i++) {
    if (_throttles[i]->speedChanged() || _throttles[i]->directionChanged() || _throttles[i]->locoChanged()) {
      _displayInterface->updateThrottleScreen(i, _throttles[i]);
      _throttles[i]->resetSpeedChanged();
      _throttles[i]->resetDirectionChanged();
      _throttles[i]->resetLocoChanged();
    }
  }
}

bool AppOrchestrator::_isLocoAddressAssociated(int address) {
  if (!_throttles)
    return false;

  for (int i = 0; i < NUM_THROTTLES; i++) {
    if (_throttles[i]->getLoco() && _throttles[i]->getLoco()->getAddress() == address) {
      return true;
    } else if (_throttles[i]->getConsist() != nullptr) {
      for (ConsistLoco *cLoco = _throttles[i]->getConsist()->getFirst(); cLoco; cLoco = cLoco->getNext()) {
        if (cLoco->getLoco()->getAddress() == address) {
          return true;
        }
      }
    }
  }
  return false;
}
