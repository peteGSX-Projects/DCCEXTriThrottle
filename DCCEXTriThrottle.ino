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

RotaryEncoder *encoder1 = new RotaryEncoder(PC14, PC15, RotaryEncoderMode::HalfStep);
RotaryEncoder *encoder2 = new RotaryEncoder(PB0, PB1, RotaryEncoderMode::FullStep);
RotaryEncoder *encoder3 = new RotaryEncoder(PB10, PB11, RotaryEncoderMode::HalfStep);

void setup() {
  Serial.begin(115200);
  Serial.println("DCC-EX Tri Throttle");
  encoder1->begin();
  encoder2->begin();
  encoder3->begin();
}

void loop() {
  if (encoder1->check() == RotaryEncoderDirection::CW) {
    Serial.println("Encoder1 - CW");
  } else if (encoder1->check() == RotaryEncoderDirection::CCW) {
    Serial.println("Encoder1 - CCW");
  }
  if (encoder2->check() == RotaryEncoderDirection::CW) {
    Serial.println("Encoder2 - CW");
  } else if (encoder2->check() == RotaryEncoderDirection::CCW) {
    Serial.println("Encoder2 - CCW");
  }
  if (encoder3->check() == RotaryEncoderDirection::CW) {
    Serial.println("Encoder3 - CW");
  } else if (encoder3->check() == RotaryEncoderDirection::CCW) {
    Serial.println("Encoder3 - CCW");
  }
}
