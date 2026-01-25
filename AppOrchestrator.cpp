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
#include "Version.h"

AppOrchestrator::AppOrchestrator(DisplayInterface *displayInterface, UserInputInterface *userInputInterface,
                                 Logger *logger, int numThrottles, Throttle **throttles,
                                 ConnectionManager *connectionManager, EventManager *eventManager,
                                 MenuManager *menuManager, DCCEXProtocol *commandStationClient)
    : _displayInterface(displayInterface), _userInputInterface(userInputInterface), _logger(logger),
      _numThrottles(numThrottles), _throttles(throttles), _connectionManager(connectionManager),
      _eventManager(eventManager), _menuManager(menuManager), _commandStationClient(commandStationClient) {
  LOG(LogLevel::LOG_DEBUG, "AppOrchestrator() created");
  _currentAppState = AppState::Startup;
  _activeContextIndex = -1;
  _enterAddressBuffer = 0;
  _enterAddressBufferCount = 0;
}

void AppOrchestrator::begin() {
  LOG(LogLevel::LOG_DEBUG, "AppOrchestrator::begin()");

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
  switch (_currentAppState) {
  case AppState::Startup: {
    _handleStartupState();
    break;
  }
  case AppState::Throttle: {
    _handleThrottleState(inputEvent);
    for (int i = 0; i < _numThrottles; i++) {
      _throttles[i]->update();
    }
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
  default: {
    LOG(LogLevel::LOG_ERROR, "AppOrchestrator::update(): Unknown AppState");
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
  LOG(LogLevel::LOG_DEBUG, "AppOrchestrator::onEvent(): %d", (int)event.eventType);

  /**
   * @brief Define the function pointer table for all event handlers using the typedef signature
   * @details Every slot must have a valid pointer, so this must be kept in sync with the EventType enum in
   * EventStructure.h
   */
  static const EventHandler eventHandlers[] = {
      &AppOrchestrator::_handleCommandStationConnected, // 0
      &AppOrchestrator::_handleReceivedRosterList,      // 1
      &AppOrchestrator::_handleLocoSelected,            // 2
      &AppOrchestrator::_handleReceivedLocoUpdate,      // 3
      &AppOrchestrator::_handleReceivedTrackPower,      // 4
      &AppOrchestrator::_handleReceivedReadLoco,        // 5
      &AppOrchestrator::_handleToggleTrackPower,        // 6
      &AppOrchestrator::_handleReceivedLocoBroadcast,   // 7
      &AppOrchestrator::_handleConnectionRetry,         // 8
      &AppOrchestrator::_handleReadLocoRetry,           // 9
      &AppOrchestrator::_handleExitMenu,                // 10
      &AppOrchestrator::_handleMenuRefreshRequired,     // 11
      &AppOrchestrator::_handleLocoAddressEntered,      // 12
      &AppOrchestrator::_handleRequestStateChange,      // 13
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
    LOG(LogLevel::LOG_ERROR, "AppOrchestrator::onEvent() unknown event: %d", typeIndex);
  }
}

AppState AppOrchestrator::getCurrentAppState() { return _currentAppState; }

void AppOrchestrator::setCurrentAppState(AppState state) { _currentAppState = state; }

int AppOrchestrator::getActiveContextIndex() { return _activeContextIndex; }

void AppOrchestrator::setActiveContextIndex(int index) { _activeContextIndex = index; }

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
  switch (event.key) {
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

// onEvent() handlers

void AppOrchestrator::_handleCommandStationConnected(Event &event) {
  if (_commandStationClient != nullptr) {
    Loco *roster = _commandStationClient->roster->getFirst();
    if (roster->getFirst()) {
      LOG(LogLevel::LOG_DEBUG, "AppOrchestrator: _connectionManager->createRosterMenu(%s)", roster->getName());
      _menuManager->createRosterMenu(roster);
    } else {
      LOG(LogLevel::LOG_DEBUG, "Empty roster, cannot create menu");
    }
  }
}

void AppOrchestrator::_handleReceivedRosterList(Event &event) {}

void AppOrchestrator::_handleLocoSelected(Event &event) {
  if (_throttles) {
    int throttleIndex = event.eventData.selectLocoValue.throttleIndex;
    Loco *newLoco = event.eventData.selectLocoValue.loco;
    // If the current loco is entered manually, delete it first
    Loco *currentLoco = _throttles[throttleIndex]->getLoco();
    if (currentLoco != nullptr && currentLoco->getSource() == LocoSource::LocoSourceEntry) {
      delete currentLoco;
    }
    _throttles[throttleIndex]->setLoco(newLoco);
    _menuManager->reset();
    _switchState(AppState::Throttle);
  }
}

void AppOrchestrator::_handleReceivedLocoUpdate(Event &event) {}

void AppOrchestrator::_handleReceivedTrackPower(Event &event) {}

void AppOrchestrator::_handleReceivedReadLoco(Event &event) {}

void AppOrchestrator::_handleToggleTrackPower(Event &event) {}

void AppOrchestrator::_handleReceivedLocoBroadcast(Event &event) {
  LocoBroadcast broadcast = event.eventData.locoBroadcastValue;

  // Need to iterate through each throttle to find the Loco
  for (int i = 0; i < _numThrottles; i++) {
    Loco *loco;
    if (_throttles[i]->getLoco() != nullptr) {
      // If it's a loco, easy
      loco = _throttles[i]->getLoco();
    } else if (_throttles[i]->getConsist() != nullptr) {
      // If it's a consist, we only care about the first one
      loco = _throttles[i]->getConsist()->getFirst()->getLoco();
    } else {
      // Continue to the next throttle if no Loco
      continue;
    }
    // If it's a roster loco, it's already managed
    if (loco->getSource() == LocoSource::LocoSourceRoster)
      continue;

    if (loco->getAddress() == broadcast.address) {
      loco->setSpeed(broadcast.speed);
      loco->setDirection(broadcast.direction);
      loco->setFunctionStates(broadcast.functionMap);
    }
  }
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
  int address = event.eventData.locoAddressValue.address;
  int throttleIndex = event.eventData.locoAddressValue.throttleIndex;

  // Need to validate DCC address first, redirect with an error if invalid
  if (address < 1 || address > 10239) {
    LOG(LogLevel::LOG_WARN, "AppOrchestrator:: Invalid DCC address entered: %d", address);
    _switchState(AppState::EnterLocoAddress);
    _displayInterface->displayUserEntryScreen("Enter Address", "Invalid address! Retry:");
  } else {
    // Otherwise create the new loco with the address as the name and associate it
    Loco *loco = new Loco(address, LocoSource::LocoSourceEntry);
    char name[6];
    snprintf(name, sizeof(name), "%d", address);
    loco->setName(name);
    Event selectEvent(EventType::LocoSelected, EventData(loco, throttleIndex));
    _handleLocoSelected(selectEvent);
  }
}

void AppOrchestrator::_handleRequestStateChange(Event &event) {
  AppState newState = event.eventData.stateRequestValue.state;
  _activeContextIndex = event.eventData.stateRequestValue.contextIndex;
  _switchState(newState);
}

// General helper methods

void AppOrchestrator::_switchState(AppState newState) {
  if (newState >= AppState::APP_STATE_COUNT)
    return;

  if (_currentAppState == newState)
    return;

  LOG(LogLevel::LOG_DEBUG, "AppOrchestrator::_switchState(%d)", newState);
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
    _displayInterface->displayThrottleScreen(_throttles);
    break;
  }
  case AppState::ConnectionError: {
    _displayInterface->displayConnectionErrorScreen();
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
