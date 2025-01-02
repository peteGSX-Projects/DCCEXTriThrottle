/*
 *  © 2023 Peter Cole
 *  © 2022 Peter Cole
 *
 *  This file is for a serially connected throttle for a DCC-EX EX-CommandStation.
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

/***********************************************************************************
Include the required libraries
***********************************************************************************/
#include <Arduino.h>
#include "defines.h"
#include "DisplayFunctions.h"
#include "InputFunctions.h"
#include "SerialFunctions.h"
#include "DCCEXObjects.h"
#include "DeviceFunctions.h"

unsigned long ramDelay=5000;
unsigned long lastRamDisplay=0;

/***********************************************************************************
Main setup function
***********************************************************************************/
void setup() {
#if defined(ARDUINO_BLUEPILL_F103C8)
  disableJTAG();
#endif
  // Set up serial and display basic config
  CONSOLE.begin(115200);
#if defined(ARDUINO_BLUEPILL_F103C8) || defined(ARDUINO_BLACKPILL_F411CE)
  CLIENT.begin(115200);
#endif
  displayStartupInfo();
  delay(2000);
  setupKeypad();
  setupThrottles();
  createMenus();
  dccexProtocol.setLogStream(&CONSOLE);
  dccexProtocol.setDelegate(&dccexCallbacks);
  dccexProtocol.connect(&CLIENT);
}

/***********************************************************************************
Main loop
***********************************************************************************/
void loop() {
  // if (millis()-lastRamDisplay>ramDelay) {
  //   lastRamDisplay=millis();
  //   CONSOLE.print("Free RAM: ");
  //   CONSOLE.print(getFreeMemory());
  //   CONSOLE.println(" bytes");
  // }
  dccexProtocol.check();
  getDCCEXObjects();
  for (int i=0; i<NUM_THROTTLES; i++) {
    if (throttles[i]->speedChanged()) {
      displayThrottleSpeed(i, throttles[i]->getSpeed(), menuSystem.isHome());
    }
    if (throttles[i]->directionChanged()) {
      displayThrottleDirection(i, throttles[i]->getDirection(), menuSystem.isHome());
    }
    throttles[i]->process();
  }
  keypad.getKey();
  getSerialInput();
}
