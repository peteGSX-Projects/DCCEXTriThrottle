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
#include "DisplayInterface.h"
#include "EventListener.h"
#include "Logger.h"
#include "UserInputInterface.h"

// State machine states enum
enum class AppState { Startup };

/**
 * @brief The AppOrchestrator coordinates all application activity using a state machine and responding to
 * user input. This extends the EventListener to respond to events when triggered.
 */
class AppOrchestrator : public EventListener {
public:
  AppOrchestrator(DisplayInterface *displayInterface, UserInputInterface *UserInputInterface, Logger *logger);

  void begin();

  void update();

  void onEvent(Event &event);

private:
  DisplayInterface *_displayInterface;
  UserInputInterface *_userInputInterface;
  Logger *_logger;
};

#endif // APPORCHESTRATOR_H
