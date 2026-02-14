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

#ifndef COMMANDSTATIONLISTENER_H
#define COMMANDSTATIONLISTENER_H

#include "EventManager.h"
#include "Logger.h"
#include <DCCEXProtocol.h>

class CommandStationListener : public DCCEXProtocolDelegate {
public:
  /**
   * @brief Construct a new Command Station Listener object
   * @param eventManager Pointer to the EventManager instance for the application
   * @param logger Pointer to the application Logger instance
   */
  CommandStationListener(EventManager *eventManager, Logger *logger);

  /**
   * @brief Publish a track power event
   * @param powerState TrackPower
   */
  void receivedTrackPower(TrackPower powerState) override;

  /**
   * @brief Destroy the Command Station Listener object
   */
  virtual ~CommandStationListener() = default;

private:
  EventManager *_eventManager;
  Logger *_logger;
};

#endif // COMMANDSTATIONLISTENER_H
