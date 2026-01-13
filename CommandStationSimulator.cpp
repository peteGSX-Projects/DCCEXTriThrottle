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

#include "CommandStationSimulator.h"

CommandStationSimulator::CommandStationSimulator(EventManager *eventManager) : _eventManager(eventManager) {}

void CommandStationSimulator::begin() { _createSimulatedRoster(); }

CommandStationSimulator::~CommandStationSimulator() { Loco::clearRoster(); }

void CommandStationSimulator::_createSimulatedRoster() {
  Loco *loco3 = new Loco(3, LocoSource::LocoSourceRoster);
  loco3->setName("Demo Loco 3");
  Loco *loco300 = new Loco(300, LocoSource::LocoSourceRoster);
  loco300->setName("Demo Loco 300");
  Loco *loco1234 = new Loco(1234, LocoSource::LocoSourceRoster);
  loco1234->setName("Demo Loco 1234");
  EventData eventData;
  _eventManager->publish(EventType::ReceivedRosterList, eventData);
}
