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

ConnectionManager::ConnectionManager(DCCEXProtocol *commandStationClient)
    : _commandStationClient(commandStationClient), _connectionState(ConnectionState::None), _lastRetry(0),
      _retriesRemaining(0) {}

void ConnectionManager::begin() {
  printf("begin(): getLists()\n");
  _connectionState = ConnectionState::Connecting;
  _retriesRemaining = CONNECT_RETRIES;
  _lastRetry = millis();
  _commandStationClient->getLists(true, true, true, true);
}

void ConnectionManager::update() {
  if (_connectionState != ConnectionState::Connecting)
    return;

  if (_commandStationClient->receivedLists()) {
    _connectionState = ConnectionState::Connected;
    return;
  }

  unsigned long currentMillis = millis();
  if (currentMillis - _lastRetry >= CONNECT_RETRY_DELAY) {
    if (_retriesRemaining == 0) {
      printf("update(): Retries exceeded, failing\n");
      _connectionState = ConnectionState::Failed;
      return;
    }

    printf("update(): Retrying, retries left: %d\n", _retriesRemaining);
    _retriesRemaining--;
    _lastRetry = currentMillis;
    _commandStationClient->getLists(true, true, true, true);
  }
}

ConnectionState ConnectionManager::getState() { return _connectionState; }

ConnectionManager::~ConnectionManager() {}
