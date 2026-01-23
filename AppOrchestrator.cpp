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
}

void AppOrchestrator::begin() {
  LOG(LogLevel::LOG_DEBUG, "AppOrchestrator::begin()");

  // Subscribe to events first
  _eventManager->subscribe(this, EventType::CommandStationConnected);
  _eventManager->subscribe(this, EventType::ReceivedRosterList);
  _eventManager->subscribe(this, EventType::LocoSelected);
  _eventManager->subscribe(this, EventType::ReceivedLocoUpdate);
  _eventManager->subscribe(this, EventType::ReceivedTrackPower);
  _eventManager->subscribe(this, EventType::ReceivedReadLoco);
  _eventManager->subscribe(this, EventType::ToggleTrackPower);
  _eventManager->subscribe(this, EventType::ReceivedLocoBroadcast);
  _eventManager->subscribe(this, EventType::ConnectionRetry);
  _eventManager->subscribe(this, EventType::ReadLocoRetry);
  _eventManager->subscribe(this, EventType::ExitMenu);
  _eventManager->subscribe(this, EventType::MenuRefreshRequired);

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
  LOG(LogLevel::LOG_DEBUG, "AppOrchestrator::onEvent(): %s", eventTypeToString(event.eventType));
  EventType eventType = event.eventType;
  switch (eventType) {
  case EventType::CommandStationConnected: {
    if (_commandStationClient != nullptr) {
      Loco *roster = _commandStationClient->roster->getFirst();
      if (roster->getFirst()) {
        LOG(LogLevel::LOG_DEBUG, "AppOrchestrator: _connectionManager->createRosterMenu(%s)", roster->getName());
        _menuManager->createRosterMenu(roster);
      } else {
        LOG(LogLevel::LOG_DEBUG, "Empty roster, cannot create menu");
      }
    }
    break;
  }
  case EventType::ConnectionRetry: {
    if (_currentAppState == AppState::Startup) {
      _displayInterface->updateProgressScreen();
    }
    break;
  }
  case EventType::ExitMenu: {
    _switchState(AppState::Throttle);
    break;
  }
  case EventType::MenuRefreshRequired: {
    _displayInterface->setRedraw(true);
    break;
  }
  case EventType::ReceivedRosterList: {
    break;
  }
  case EventType::LocoSelected: {
    if (_throttles) {
      int throttleIndex = event.eventData.selectLocoValue.throttleIndex;
      Loco *loco = event.eventData.selectLocoValue.loco;
      _throttles[throttleIndex]->setLoco(loco);
      _switchState(AppState::Throttle);
    }
  }
  default: {
    LOG(LogLevel::LOG_ERROR, "AppOrchestrator::onEvent(): Unknown Event received");
  }
  }
}

AppState AppOrchestrator::getCurrentAppState() { return _currentAppState; }

AppOrchestrator::~AppOrchestrator() {}

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
    LOG(LogLevel::LOG_DEBUG, "AppOrchestrator::_handleThrottleState() unhandled key pressed: %c", event.key);
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

void AppOrchestrator::_switchState(AppState newState) {
  if (_currentAppState == newState)
    return;
  LOG(LogLevel::LOG_DEBUG, "AppOrchestrator::_switchState(%s)", _appStateToString(newState));
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
    _displayInterface->displayThrottleScreen();
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
  default:
    break;
  }
}

void AppOrchestrator::_updateThrottleDisplay() {
  for (int i = 0; i < _numThrottles; i++) {
    if (_throttles[i]->speedChanged()) {
      _displayInterface->updateThrottleScreen(i, _throttles[i]);
    }
    if (_throttles[i]->directionChanged()) {
      _displayInterface->updateThrottleScreen(i, _throttles[i]);
    }
    if (_throttles[i]->locoChanged()) {
      _displayInterface->updateThrottleScreen(i, _throttles[i]);
    }
  }
}

const char *AppOrchestrator::_appStateToString(AppState appState) {
  switch (appState) {
  case AppState::Startup:
    return "Startup";
  case AppState::Throttle:
    return "Throttle";
  case AppState::ConnectionError:
    return "ConnectionError";
  case AppState::Menu:
    return "Menu";
  default:
    return "UNKNOWN";
  }
}
