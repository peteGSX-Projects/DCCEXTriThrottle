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

#ifndef COMMANDSTATIONSIMULATOR_H
#define COMMANDSTATIONSIMULATOR_H

#include "EventManager.h"
#include <DCCEXProtocol.h>

class CommandStationSimulator {
public:
  /**
   * @brief Construct a new Command Station Simulator object
   * @param eventManager Pointer to the application EventManager
   */
  CommandStationSimulator(EventManager *eventManager);

  /**
   * @brief Instantiate all simulated objects
   */
  void begin();

  /**
   * @brief Destroy the Command Station Simulator object
   */
  ~CommandStationSimulator();

private:
  EventManager *_eventManager;

  /**
   * @brief Create simulated roster entries and publish the list is received
   */
  void _createSimulatedRoster();
};

#endif // COMMANDSTATIONSIMULATOR_H
