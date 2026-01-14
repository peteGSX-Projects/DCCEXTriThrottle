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

#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include "Defines.h"
#include "EventManager.h"
#include <DCCEXProtocol.h>

/**
 * @brief Enumeration of valid connection states
 */
enum class ConnectionState { None, Connecting, Connected, Failed, DemoMode };

/**
 * @brief Class to manage connections to the command station
 */
class ConnectionManager {
public:
  /**
   * @brief Construct a new Connection Manager object
   * @param commandStationClient Pointer to the DCCEXProtocol instance to use as the client
   * @param eventManager Pointer to the application EventManager instance
   * @param logger Pointer to the application Logger instance
   */
  ConnectionManager(DCCEXProtocol *commandStationClient, EventManager *eventManager, Logger *logger);

  /**
   * @brief Initialise the client
   */
  virtual void begin();

  /**
   * @brief Call this method at least once per main loop iteration once connected
   */
  virtual void update();

  /**
   * @brief Get the current ConnectionState
   * @return ConnectionState Current ConnectionState of the client
   */
  virtual ConnectionState getState();

  /**
   * @brief Destroy the Connection Manager object
   */
  virtual ~ConnectionManager();

private:
  DCCEXProtocol *_commandStationClient;
  EventManager *_eventManager;
  Logger *_logger;
  ConnectionState _connectionState;
  unsigned long _lastRetry;
  int _retriesRemaining;
};

#endif // CONNECTIONMANAGER_H
