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

#include "CommandStationListener.h"

CommandStationListener::CommandStationListener(EventManager *eventManager, Logger *logger)
    : _eventManager(eventManager), _logger(logger) {}

void CommandStationListener::receivedLocoBroadcast(int address, int speed, Direction direction, int functionMap) {
  if (_eventManager) {
    LocoBroadcast broadcast = {address, speed, direction, functionMap};
    EventData eventData(broadcast);
    LOG(LogLevel::LOG_DEBUG, "CommandStationListener::receivedLocoBroadcast(): published event for address: ", address);
    _eventManager->publish(EventType::ReceivedLocoBroadcast, eventData);
  }
}

void CommandStationListener::receivedTrackPower(TrackPower powerState) {
  if (_eventManager) {
    EventData eventData(powerState);
    LOG(LogLevel::LOG_DEBUG, "CommandStationListener::receivedTrackPower(): published event");
    _eventManager->publish(EventType::ReceivedTrackPower, eventData);
  }
}
