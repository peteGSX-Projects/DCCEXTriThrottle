/*
 *  © 2026 Peter Cole
 *  © 2025 Peter Cole
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

/**
 * @file DCCEXTriThrottle.ino
 * @mainpage DCC-EX Tri-Throttle
 * @brief A throttle for DCC-EX to simultaneously control three locos via the DCC-EX Protocol
 *
 * @details
 *
 */

// Includes in both production and testing
#include "Defines.h"
#include "Logger.h"

// Don't do standard Arduino stuff if testing
#if !defined(NATIVE_TESTING) && !defined(DEVICE_TESTING)

#include "AppConfiguration.h"
#include <Arduino.h>

AppConfiguration *appConfiguration = nullptr;

void setup() {
  CONSOLE_STREAM.begin(115200);
  COMMANDSTATION_STREAM.begin(115200);
  delay(3000);
  // Instantiate config only after stream begin() is called
  appConfiguration = new AppConfiguration(&CONSOLE_STREAM, &COMMANDSTATION_STREAM, LOG_LEVEL);
  appConfiguration->initialise();
  LOG(LogLevel::LOG_MESSAGE, "DCC-EX Tri-Throttle");
  AppOrchestrator *appOrchestrator = appConfiguration->getAppOrchestrator();
  appOrchestrator->begin();
}

void loop() {
  AppOrchestrator *appOrchestrator = appConfiguration->getAppOrchestrator();
  appOrchestrator->update();
}

// Include if doing device testing
#elif defined(DEVICE_TESTING)
#warning "Device testing mode is enabled"
#include "TestUserInteraction.h"

#endif // !defined(NATIVE_TESTING) && !defined(DEVICE_TESTING)
