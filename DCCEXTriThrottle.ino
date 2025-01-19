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
 * @brief
 *
 * @details
 *
 */

#include "RotaryEncoder.h"
#include <Arduino.h>

RotaryEncoder *encoder1 = new RotaryEncoder(PC14, PC15, RotaryEncoder::Mode::HalfStep);
RotaryEncoder *encoder2 = new RotaryEncoder(PB0, PB1, RotaryEncoder::Mode::HalfStep);
RotaryEncoder *encoder3 = new RotaryEncoder(PB10, PB11, RotaryEncoder::Mode::HalfStep);

void setup() {
  Serial.begin(115200);
  Serial.println("DCC-EX Tri Throttle");
  encoder1->begin();
  encoder2->begin();
  encoder3->begin();
}

void loop() {
  RotaryEncoder::Direction enc1 = encoder1->check();
  if (enc1 == RotaryEncoder::Direction::CW) {
    Serial.println("Encoder1 - CW");
  } else if (enc1 == RotaryEncoder::Direction::CCW) {
    Serial.println("Encoder1 - CCW");
  }
  RotaryEncoder::Direction enc2 = encoder2->check();
  if (enc2 == RotaryEncoder::Direction::CW) {
    Serial.println("Encoder2 - CW");
  } else if (enc2 == RotaryEncoder::Direction::CCW) {
    Serial.println("Encoder2 - CCW");
  }
  RotaryEncoder::Direction enc3 = encoder3->check();
  if (enc3 == RotaryEncoder::Direction::CW) {
    Serial.println("Encoder3 - CW");
  } else if (enc3 == RotaryEncoder::Direction::CCW) {
    Serial.println("Encoder3 - CCW");
  }
}
