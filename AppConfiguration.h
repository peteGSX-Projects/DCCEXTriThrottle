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

#ifndef APPCONFIGURATION_H
#define APPCONFIGURATION_H

/** Includes here */
#include "Button.h"
#include "CustomisableKeypad.h"
#include "DisplayInterface.h"
#include "EventManager.h"
#include "Logger.h"
#include "RotaryEncoder.h"
#include "Throttle.h"
#include "U8G2SH1106Display.h"
#include "UserInputInterface.h"

/** Set the number of throttles here, unlikely to change but possibly may later */
static const int NUM_THROTTLES = 3;

/**
 * @brief AppConfiguration performs all application configuration on startup.
 * All interfaces, menus, DCCEXProtocol related objects, and other application objects are created within this class.
 */
class AppConfiguration {
public:
  /**
   * @brief Construct a new App Configuration object - ensure Stream objects are started before instantiating!
   * @param consoleStream Pointer to the stream for console interaction
   * @param commandStationStream Pointer to the stream for the CommandStation connection
   * @param logLevel Log level to set (default is WARN if not specified)
   */
  AppConfiguration(Stream *consoleStream, Stream *commandStationStream, LogLevel logLevel = LogLevel::LOG_WARN);

  /**
   * @brief Initialise all associated interfaces/objects and register events
   */
  void initialise();

  /**
   * @brief Get the Console Stream object
   * @return Stream* 
   */
  Stream *getConsoleStream();

  /**
   * @brief Get the Command Station Stream object
   * @return Stream* 
   */
  Stream *getCommandStationStream();

  /**
   * @brief Get the User Input Interface object
   * @return UserInputInterface*
   */
  UserInputInterface *getUserInputInterface();

  /**
   * @brief Get the Display Interface object
   * @return DisplayInterface*
   */
  DisplayInterface *getDisplayInterface();

  /**
   * @brief Get the Event Manager object
   * @return EventManager*
   */
  EventManager *getEventManager();

  /**
   * @brief Get the Logger object
   * @return Logger*
   */
  Logger *getLogger();

  /**
   * @brief Get the Throttles object
   * @return Throttle**
   */
  Throttle **getThrottles();

  /**
   * @brief Destroy the App Configuration object
   */
  ~AppConfiguration();

private:
  Stream *_consoleStream;
  Stream *_commandStationStream;
  UserInputInterface *_userInputInterface;
  DisplayInterface *_displayInterface;
  EventManager *_eventManager;
  Logger *_logger;
  Throttle *_throttles[NUM_THROTTLES];

#if defined(ARDUINO_BLUEPILL_F103C8)
  void _disableJTAG();
#endif // BLUEPILL
};

#endif // APPCONFIGURATION_H
