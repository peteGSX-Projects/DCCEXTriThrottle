/*
 *  © 2023 Peter Cole
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

#include "SerialFunctions.h"

const uint8_t numBytes = 100;             // Maximum number of serial bytes to accept for input.
byte serialInputBytes[numBytes];          // Byte array for serial bytes received.
bool newSerialData = false;               // Flag for new serial data being received.

/***********************************************************************************
Serial monitoring/processing functions
***********************************************************************************/
void getSerialInput() {
  static bool serialInProgress = false;
  static uint8_t serialIndex = 0;
  byte startMarker = '<';
  byte endMarker = '>';
  byte serialByte;
  while (CONSOLE.available() > 0 && newSerialData == false) {
    serialByte = CONSOLE.read();
    if (serialInProgress == true) {
      if (serialByte != endMarker) {
        serialInputBytes[serialIndex] = serialByte;
        serialIndex++;
        if (serialIndex >= numBytes) {
          serialIndex = numBytes - 1;
        }
      } else {
        serialInputBytes[serialIndex] = '\0';
        serialInProgress = false;
        serialIndex = 0;
        newSerialData = true;
      }
    } else if (serialByte == startMarker) {
      serialInProgress = true;
    }
  }
  if (newSerialData == true) {
    CONSOLE.println(F("Got serial command"));
    CLIENT.print(F("<"));
    for (uint8_t i = 0; i < numBytes; i++) {
      if (serialInputBytes[i] == '\0') break;
      CONSOLE.print((char)serialInputBytes[i]);
      CLIENT.print((char)serialInputBytes[i]);
    }
    CONSOLE.println(F(""));
    CLIENT.println(F(">"));
    newSerialData = false;
  }
}
