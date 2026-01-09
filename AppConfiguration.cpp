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

#include "AppConfiguration.h"
#include "Version.h"

AppConfiguration::AppConfiguration(Stream *consoleStream, Stream *commandStationStream, LogLevel logLevel)
    : _consoleStream(consoleStream), _commandStationStream(commandStationStream) {
  _appOrchestrator = nullptr;
  _userInputInterface = nullptr;
  _displayInterface = nullptr;
  _eventManager = new EventManager;
  _logger = new Logger;
  _logger->setLogLevel(logLevel);
  _logger->setOutput(_consoleStream);
  for (int i = 0; i < NUM_THROTTLES; i++) {
    _throttles[i] = nullptr;
  }
}

void AppConfiguration::initialise() {
#ifndef NATIVE_TESTING
  LOG(LogLevel::LOG_DEBUG, "AppConfiguration::initialise()");
  // Initialise the keypad first and define the required keypad arrays using user defines
  static const byte testRowPins[] = {KEYPAD_ROW_PINS};
  static const byte testColumnPins[] = {KEYPAD_COLUMN_PINS};
  static const char testKeyMap[] = {KEYPAD_MAP};
  _userInputInterface = new CustomisableKeypad(KEYPAD_ROWS, KEYPAD_COLUMNS, testRowPins, testColumnPins, testKeyMap,
                                               KEYPAD_DEBOUNCE_TIME, KEYPAD_HELD_THRESHOLD);
  _userInputInterface->begin();

  // Display is next
  _displayInterface = new U8G2SH1106Display();
  _displayInterface->begin();

  // Now set up throttles, each with its own rotary encoder and button
  UserConfirmationInterface *button1 = new Button(ENCODER1_BUTTON);
  UserConfirmationInterface *button2 = new Button(ENCODER2_BUTTON);
  UserConfirmationInterface *button3 = new Button(ENCODER3_BUTTON);

  // Set rotary encoder mode according to user selection
  RotaryEncoder::Mode encoderMode =
      (ENCODER_MODE == HALF_STEP) ? RotaryEncoder::Mode::HalfStep : RotaryEncoder::Mode::FullStep;
  UserSelectionInterface *encoder1 = new RotaryEncoder(ENCODER1_DT, ENCODER1_CLK, encoderMode);
  UserSelectionInterface *encoder2 = new RotaryEncoder(ENCODER2_DT, ENCODER2_CLK, encoderMode);
  UserSelectionInterface *encoder3 = new RotaryEncoder(ENCODER3_DT, ENCODER3_CLK, encoderMode);

  // Now create the throttle instances
  _throttles[0] = new Throttle(button1, encoder1, THROTTLE_STEP, THROTTLE_STEP_FASTER, THROTTLE_STEP_FASTEST);
  _throttles[1] = new Throttle(button2, encoder2, THROTTLE_STEP, THROTTLE_STEP_FASTER, THROTTLE_STEP_FASTEST);
  _throttles[2] = new Throttle(button3, encoder3, THROTTLE_STEP, THROTTLE_STEP_FASTER, THROTTLE_STEP_FASTEST);
#endif // NATIVE_TESTING

  // Lastly, create orchestrator instance
  _appOrchestrator = new AppOrchestrator(_displayInterface, _userInputInterface, _logger, _throttles);
}

AppOrchestrator *AppConfiguration::getAppOrchestrator() { return _appOrchestrator; }

Stream *AppConfiguration::getConsoleStream() { return _consoleStream; }

Stream *AppConfiguration::getCommandStationStream() { return _commandStationStream; }

UserInputInterface *AppConfiguration::getUserInputInterface() { return _userInputInterface; }

DisplayInterface *AppConfiguration::getDisplayInterface() { return _displayInterface; }

EventManager *AppConfiguration::getEventManager() { return _eventManager; }

Logger *AppConfiguration::getLogger() { return _logger; }

Throttle **AppConfiguration::getThrottles() { return _throttles; }

AppConfiguration::~AppConfiguration() {
  delete _eventManager;
  delete _logger;

  if (_userInputInterface)
    delete _userInputInterface;
  if (_displayInterface)
    delete _displayInterface;

  for (int i = 0; i < NUM_THROTTLES; i++) {
    if (_throttles[i]) {
      delete _throttles[i];
    }
  }
}

#if defined(ARDUINO_BLUEPILL_F103C8)
void AppConfiguration::_disableJTAG() {
  // Disable JTAG and enable SWD by clearing the SWJ_CFG bits
  // Assuming the register is named AFIO_MAPR or AFIO_MAPR2
  AFIO->MAPR &= ~(AFIO_MAPR_SWJ_CFG);
  // or
  // AFIO->MAPR2 &= ~(AFIO_MAPR2_SWJ_CFG);
}
#endif // BLUEPILL
