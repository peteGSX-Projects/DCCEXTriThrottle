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
#include "HardwareManager.h"
#include "Logger.h"
#include "Version.h"

// Don't do standard Arduino stuff if testing
#if !defined(NATIVE_TESTING) && !defined(DEVICE_TESTING)

#include "AppOrchestrator.h"
#include "Button.h"
#include "CommandStationListener.h"
#include "ConnectionManager.h"
#include "CustomisableKeypad.h"
#include "EventManager.h"
#include "RotaryEncoder.h"
#include "Throttle.h"
#include "U8G2SH1106Display.h"
#include <Arduino.h>

// Global orchestrator
AppOrchestrator *orchestrator = nullptr;

void setup() {
  // Setup hardware
  static HardwareManager hardwareManager;
  hardwareManager.initialise();

  // Setup logger
  static Logger logger;
  logger.setLogLevel(LOG_LEVEL);
  logger.setOutput(&CONSOLE_STREAM);

#ifdef STARTUP_DELAY
  delay(STARTUP_DELAY);
#endif // STARTUP_DELAY

  // Setup application services
  static EventManager eventManager(&logger);
  static DCCEXProtocol commandStationClient;
  static CommandStationListener commandStationListener(&eventManager, &logger);
  commandStationClient.setLogStream(&CONSOLE_STREAM);
  commandStationClient.setDelegate(&commandStationListener);
  commandStationClient.connect(&COMMANDSTATION_STREAM);
  static ConnectionManager connectionManager(&commandStationClient, &eventManager, &logger);
  static MenuManager menuManager(&eventManager, &logger);

  // Setup keypad
  static const byte keypadRowPins[] = {KEYPAD_ROW_PINS};
  static const byte keypadColumnPins[] = {KEYPAD_COLUMN_PINS};
  static const char keypadKeyMap[] = {KEYPAD_MAP};
  static CustomisableKeypad keypad(KEYPAD_ROWS, KEYPAD_COLUMNS, keypadRowPins, keypadColumnPins, keypadKeyMap,
                                   KEYPAD_DEBOUNCE_TIME, KEYPAD_HELD_THRESHOLD);

  // Setup display
  static U8G2SH1106Display display(NUM_THROTTLES);

  // Setup throttles
  static RotaryEncoder::Mode encoderMode =
      (ENCODER_MODE == HALF_STEP) ? RotaryEncoder::Mode::HalfStep : RotaryEncoder::Mode::FullStep;

  // Throttle array of pointers
  static Throttle *throttles[NUM_THROTTLES];

  // Throttle 0 hardware
  static Button button0(ENCODER1_BUTTON);
  static RotaryEncoder encoder0(ENCODER1_DT, ENCODER1_CLK, encoderMode);
  encoder0.begin();

  // Throttle 1 hardware
  static Button button1(ENCODER2_BUTTON);
  static RotaryEncoder encoder1(ENCODER2_DT, ENCODER2_CLK, encoderMode);
  encoder1.begin();

  // Throttle 2 hardware
  static Button button2(ENCODER3_BUTTON);
  static RotaryEncoder encoder2(ENCODER3_DT, ENCODER3_CLK, encoderMode);
  encoder2.begin();

  // Invert throttle direction if configured
#if INVERT_THROTTLE
  encoder0.setThrottleInverted();
  encoder1.setThrottleInverted();
  encoder2.setThrottleInverted();
#endif

  // Define throttles
  throttles[0] = new Throttle(0, &button0, &encoder0, THROTTLE_STEP, THROTTLE_STEP_FASTER, THROTTLE_STEP_FASTEST);
  throttles[0]->setLogger(&logger);
  throttles[1] = new Throttle(1, &button1, &encoder1, THROTTLE_STEP, THROTTLE_STEP_FASTER, THROTTLE_STEP_FASTEST);
  throttles[1]->setLogger(&logger);
  throttles[2] = new Throttle(2, &button2, &encoder2, THROTTLE_STEP, THROTTLE_STEP_FASTER, THROTTLE_STEP_FASTEST);
  throttles[2]->setLogger(&logger);

  // Setup AppOrchestrator
  static AppOrchestrator appOrchestrator(&display, &keypad, &logger, NUM_THROTTLES, throttles, &connectionManager,
                                         &eventManager, &menuManager);
  orchestrator = &appOrchestrator;
  appOrchestrator.begin();

  LOG(LogLevel::LOG_MESSAGE, "DCC-EX Tri-Throttle initialised, version %s", VERSION);
}

void loop() { orchestrator->update(); }

// Include if doing device testing
#elif defined(DEVICE_TESTING)
#warning "Device testing mode is enabled"
#include "TestUserInteraction.h"

#endif // !defined(NATIVE_TESTING) && !defined(DEVICE_TESTING)
