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
                                 ConnectionManager *connectionManager)
    : _displayInterface(displayInterface), _userInputInterface(userInputInterface), _logger(logger),
      _numThrottles(numThrottles), _throttles(throttles), _connectionManager(connectionManager) {
  LOG(LogLevel::LOG_DEBUG, "AppOrchestrator() created");
  _currentAppState = AppState::Startup;
}

void AppOrchestrator::begin() {
  LOG(LogLevel::LOG_DEBUG, "AppOrchestrator::begin()");
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
  LOG(LogLevel::LOG_DEBUG, "AppOrchestrator::onEvent(): %s", _eventTypeToString(event.eventType));
  EventType eventType = event.eventType;
  switch (eventType) {
  case EventType::CommandStationConnected: {
    break;
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

void AppOrchestrator::_handleThrottleState(UserInputInterface::UserInputEvent event) {}

void AppOrchestrator::_handleConnectionError(UserInputInterface::UserInputEvent event) {
  if (event.key == '*') {
    LOG(LogLevel::LOG_DEBUG, "Entering demo mode");
  } else if (event.key != '\0') {
    _connectionManager->begin();
    _switchState(AppState::Startup);
  }
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
    _displayInterface->displayStartupScreen("DCC-EX Tri-Throttle", VERSION);
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

const char *AppOrchestrator::_eventTypeToString(EventType eventType) {
  switch (eventType) {
  case CommandStationSelected:
    return "CommandStationSelected";
  case CommandStationConnected:
    return "CommandStationConnected";
  case ReceivedRosterList:
    return "ReceivedRosterList";
  case LocoSelected:
    return "LocoSelected";
  case ReceivedLocoUpdate:
    return "ReceivedLocoUpdate";
  case ReceivedTrackPower:
    return "ReceivedTrackPower";
  case ReceivedReadLoco:
    return "ReceivedReadLoco";
  case ToggleTrackPower:
    return "ToggleTrackPower";
  case ReceivedLocoBroadcast:
    return "ReceivedLocoBroadcast";
  default:
    return "UNKNOWN_EVENT";
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
  default:
    return "UNKNOWN";
  }
}
