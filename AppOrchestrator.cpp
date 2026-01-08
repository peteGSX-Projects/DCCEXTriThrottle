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
                                 Logger *logger)
    : _displayInterface(displayInterface), _userInputInterface(userInputInterface), _logger(logger) {
  LOG(LogLevel::LOG_DEBUG, "AppOrchestrator() created");
  _currentAppState = AppState::Startup;
}

void AppOrchestrator::update() {
  UserInputInterface::UserInputEvent inputEvent = _userInputInterface->check();
  switch (_currentAppState) {
  case AppState::Startup: {
    _handleStartupState(inputEvent);
    break;
  }
  case AppState::Throttle: {
    _handleThrottleState(inputEvent);
    break;
  }
  default: {
    LOG(LogLevel::LOG_ERROR, "AppOrchestrator::update(): Unknown AppState");
    break;
  }
  }
}

void AppOrchestrator::onEvent(Event &event) {
  LOG(LogLevel::LOG_DEBUG, "AppOrchestrator::onEvent(): %s", _eventToString(event.eventType));
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

void AppOrchestrator::_handleStartupState(UserInputInterface::UserInputEvent inputEvent) {
  _displayInterface->displayStartupScreen("DCC-EX Tri-Throttle", VERSION);
  if (inputEvent.action != UserInputInterface::UserInputAction::None) {
    _currentAppState = AppState::Throttle;
  }
}

void AppOrchestrator::_handleThrottleState(UserInputInterface::UserInputEvent inputEvent) {}

const char *AppOrchestrator::_eventToString(EventType eventType) {
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
    return "UNKNOW_EVENT";
  }
}
