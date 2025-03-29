/*
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
 * @mainpage DCC-EX Tri Throttle
 * @brief A throttle for DCC-EX to simultaneously control three locos via the DCC-EX Protocol
 *
 * @details
 *
 */

#include "Defines.h"

// Don't do standard Arduino stuff if testing
#if !defined(NATIVE_TESTING) && !defined(DEVICE_TESTING)

#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  delay(3000);
  Serial.println("DCC-EX Tri Throttle");
}

void loop() {}

// Include if doing device testing
#elif defined(DEVICE_TESTING)
#include "test_UserInteraction.h"

#endif // !defined(NATIVE_TESTING) && !defined(DEVICE_TESTING)
