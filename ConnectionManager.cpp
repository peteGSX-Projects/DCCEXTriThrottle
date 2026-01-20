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

#include "ConnectionManager.h"

ConnectionManager::ConnectionManager(DCCEXProtocol *commandStationClient, EventManager *eventManager, Logger *logger)
    : _commandStationClient(commandStationClient), _eventManager(eventManager), _logger(logger),
      _connectionState(ConnectionState::None), _lastRetry(0), _retriesRemaining(0) {}

void ConnectionManager::begin() {
  _connectionState = ConnectionState::Connecting;
  _retriesRemaining = CONNECT_RETRIES;
  _lastRetry = millis();
}

void ConnectionManager::update() {
  _commandStationClient->check();

  if (!_commandStationClient->receivedLists()) {
    _commandStationClient->getLists(true, true, true, true);
  }

  if (_connectionState != ConnectionState::Connecting)
    return;

  if (_commandStationClient->receivedLists()) {
    _connectionState = ConnectionState::Connected;
    return;
  }

  unsigned long currentMillis = millis();
  if (currentMillis - _lastRetry >= CONNECT_RETRY_DELAY) {
    if (_retriesRemaining == 0) {
      _connectionState = ConnectionState::Failed;
      return;
    }

    _retriesRemaining--;
    _lastRetry = currentMillis;
    EventData eventData;
    _eventManager->publish(EventType::ConnectionRetry, eventData);
  }
}

ConnectionState ConnectionManager::getState() { return _connectionState; }

DCCEXProtocol *ConnectionManager::getCommandStationClient() { return _commandStationClient; }

ConnectionManager::~ConnectionManager() {}
