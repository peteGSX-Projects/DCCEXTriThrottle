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
#include "AppStates.h"
#include "ConnectionManager.h"
#include "DisplayInterface.h"
#include "EventListener.h"
#include "Logger.h"
#include "MenuManager.h"
#include "Throttle.h"
#include "UserInputInterface.h"

/**
 * @brief The AppOrchestrator coordinates all application activity using a state machine and responding to
 * user input. This extends the EventListener to respond to events when triggered.
 */
class AppOrchestrator : public EventListener {
public:
  /**
   * @brief Construct a new App Orchestrator object
   * @param displayInterface Pointer to the physical implementation instance of the DisplayInterface
   * @param UserInputInterface Pointer to the physical implementation instance of the UserInputInterface (eg. keypad)
   * @param logger Pointer to the Logger instance for the application
   * @param numThrottles Number of throttles for the application (at the moment must be 3)
   * @param throttles Array of pointers to Throttle instances
   * @param connectionManager Pointer to the ConnectionManager instance for the application
   * @param eventManager Pointer to the EventManager instance for the application
   * @param menuManager Pointer to the MenuManager instance for the application
   * @param commandStationClient Pointer to the DCCEXProtocol instance for the application
   */
  AppOrchestrator(DisplayInterface *displayInterface, UserInputInterface *UserInputInterface, Logger *logger,
                  int numThrottles, Throttle **throttles, ConnectionManager *connectionManager,
                  EventManager *eventManager, MenuManager *menuManager, DCCEXProtocol *commandStationClient);

  /**
   * @brief Call any associated begin or initialisation methods
   */
  void begin();

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
   * @brief Get the Current AppState of the orchestrator
   * @return AppState Current AppState value
   */
  AppState getCurrentAppState();

  /**
   * @brief Set the Current App State for testing
   * @param state Valid AppState type
   */
  void setCurrentAppState(AppState state);

  /**
   * @brief Get the Active Context Index of the orchestrator
   * @return int Current contextual index
   */
  int getActiveContextIndex();

  /**
   * @brief Set the Active Context Index of the orchestrator
   * @param index Current contextual index
   */
  void setActiveContextIndex(int index);

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
  EventManager *_eventManager;
  MenuManager *_menuManager;
  DCCEXProtocol *_commandStationClient;
  int _activeContextIndex;
  int _enterAddressBuffer;
  int _enterAddressBufferCount;

  // update() methods
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
   * @brief Display the menu screen and enable user to interact with it
   * @param event UserInputInterface::UserInputEvent
   */
  void _handleMenuState(UserInputInterface::UserInputEvent event);

  /**
   * @brief Display the user entry screen to enable entering a Loco address
   * @param event
   */
  void _handleEnterLocoAddress(UserInputInterface::UserInputEvent event);

  // onEvent() event handlers

  /**
   * @brief
   */
  void _handleCommandStationConnected();

  /**
   * @brief
   */
  void _handleConnectionRetry();

  /**
   * @brief Handle a LocoSelected event to associate loco with a throttle
   * @param event Event containing SelectLocoData
   */
  void _handleLocoSelected(Event event);

  /**
   * @brief Handle a LocoAddressEntered event to validate the address and associate with a throttle
   * @param event Event containing LocoAddressData
   */
  void _handleLocoAddressEntered(Event event);

  /**
   * @brief Handle a RequestStateChange event typically triggered by an ActionMenuItem
   * @param event Event containing StateRequestData
   */
  void _handleRequestStateChange(Event event);

  // General helper methods

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
   * @brief Helper method to return the AppState name rather than int value
   * @param appState AppState
   * @return const char* Pointer to the char array string
   */
  const char *_appStateToString(AppState appState);
};

#endif // APPORCHESTRATOR_H
