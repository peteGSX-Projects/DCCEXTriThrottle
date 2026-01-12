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

#ifndef APPORCHESTRATOR_H
#define APPORCHESTRATOR_H

// Includes required for the orchestrator
#include "ConnectionManager.h"
#include "DisplayInterface.h"
#include "EventListener.h"
#include "Logger.h"
#include "Throttle.h"
#include "UserInputInterface.h"

/**
 * @brief Enumeration of valid state machine states
 */
enum class AppState { Startup, Throttle, ConnectionError };

/**
 * @brief The AppOrchestrator coordinates all application activity using a state machine and responding to
 * user input. This extends the EventListener to respond to events when triggered.
 */
class AppOrchestrator : public EventListener {
public:
  AppOrchestrator(DisplayInterface *displayInterface, UserInputInterface *UserInputInterface, Logger *logger,
                  int numThrottles, Throttle **throttles, ConnectionManager *connectionManager);

  /**
   * @brief Call the update() method at least once per main loop iteration
   */
  void update();

  /**
   * @brief Respond to EventListener events this instance is subscribed to
   * @param event A valid Event object
   */
  void onEvent(Event &event);

  /**
   * @brief Get the Current State object
   * @return AppState
   */
  AppState getCurrentAppState();

  /**
   * @brief Destroy the App Orchestrator object
   */
  ~AppOrchestrator();

private:
  // Attributes
  DisplayInterface *_displayInterface;
  UserInputInterface *_userInputInterface;
  Logger *_logger;
  AppState _currentAppState;
  int _numThrottles;
  Throttle **_throttles;
  ConnectionManager *_connectionManager;

  // Methods
  /**
   * @brief Display the startup screen while connecting to the CommandStation
   */
  void _handleStartupState();

  /**
   * @brief Display the throttle screen and respond to user interactions
   * @param event UserInputInterface::UserInputEvent
   */
  void _handleThrottleState(UserInputInterface::UserInputEvent event);

  /**
   * @brief Display the connection error screen and respond to user interaction
   * @param event UserInputInterface::UserInputEvent
   */
  void _handleConnectionError(UserInputInterface::UserInputEvent event);

  /**
   * @brief Switch AppState to the new state and flag a display redraw
   * @param newState New AppState to switch to
   */
  void _switchState(AppState newState);

  /**
   * @brief Display the current application state on screen
   */
  void _displayCurrentState();

  /**
   * @brief Helper method to update throttle elements only instead of full redraw
   */
  void _updateThrottleDisplay();

  /**
   * @brief Helper method to return the event type name rather than int value
   * @param eventType EventType
   * @return const char* Pointer to the char array string
   */
  const char *_eventTypeToString(EventType eventType);

  /**
   * @brief Helper method to return the AppState name rather than int value
   * @param appState AppState
   * @return const char* Pointer to the char array string
   */
  const char *_appStateToString(AppState appState);
};

#endif // APPORCHESTRATOR_H
