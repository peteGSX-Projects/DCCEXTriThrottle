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

#include "AdvancedKeypad.h"
#include "Button.h"
#include "Defines.h"
#include "RotaryEncoder.h"
#include <Arduino.h>

AdvancedKeypad keypad;
Button button1(ENCODER1_BUTTON);
Button button2(ENCODER2_BUTTON);
Button button3(ENCODER3_BUTTON);
RotaryEncoder encoder1(ENCODER1_DT, ENCODER1_CLK, RotaryEncoder::Mode::FullStep);
RotaryEncoder encoder2(ENCODER2_DT, ENCODER2_CLK, RotaryEncoder::Mode::FullStep);
RotaryEncoder encoder3(ENCODER3_DT, ENCODER3_CLK, RotaryEncoder::Mode::FullStep);

int enc1Counter = 0;
int enc2Counter = 0;
int enc3Counter = 0;

void setup() {
  Serial.begin(115200);
  keypad.begin();
  encoder1.begin();
  encoder2.begin();
  encoder3.begin();
  button1.begin();
  button2.begin();
  button3.begin();
}

void loop() {
  AdvancedKeypad::KeyEvent event = keypad.checkKeypad();
  if (event.key != '\0') {
    Serial.print("Key|Event: ");
    Serial.print(event.key);
    Serial.print("|");
    switch (event.type) {
    case AdvancedKeypad::EventType::SinglePress:
      Serial.println("SinglePress");
      break;
    case AdvancedKeypad::EventType::DoublePress:
      Serial.println("DoublePress");
      break;
    case AdvancedKeypad::EventType::LongPress:
      Serial.println("LongPress");
      break;
    default:
      break;
    }
  }
  RotaryEncoder::Direction encoder1Dir = encoder1.check();
  if (encoder1Dir == RotaryEncoder::Direction::CW) {
    enc1Counter++;
  } else if (encoder1Dir == RotaryEncoder::Direction::CCW) {
    enc1Counter--;
  }
  if (encoder1Dir != RotaryEncoder::Direction::None) {
    Serial.print("Encoder1 counter: ");
    Serial.println(enc1Counter);
  }
  RotaryEncoder::Direction encoder2Dir = encoder2.check();
  if (encoder2Dir == RotaryEncoder::Direction::CW) {
    enc2Counter++;
  } else if (encoder2Dir == RotaryEncoder::Direction::CCW) {
    enc2Counter--;
  }
  if (encoder2Dir != RotaryEncoder::Direction::None) {
    Serial.print("Encoder2 counter: ");
    Serial.println(enc2Counter);
  }
  RotaryEncoder::Direction encoder3Dir = encoder3.check();
  if (encoder3Dir == RotaryEncoder::Direction::CW) {
    enc3Counter++;
  } else if (encoder3Dir == RotaryEncoder::Direction::CCW) {
    enc3Counter--;
  }
  if (encoder3Dir != RotaryEncoder::Direction::None) {
    Serial.print("Encoder3 counter: ");
    Serial.println(enc3Counter);
  }
  Button::EventType button1Event = button1.checkButton();
  switch (button1Event) {
  case (Button::EventType::SingleClick):
    Serial.println("Button 1 SingleClick");
    break;
  case (Button::EventType::DoubleClick):
    Serial.println("Button 1 DoubleClick");
    break;
  case (Button::EventType::LongClick):
    Serial.println("Button 1 LongClick");
    break;
  case (Button::EventType::Held):
    Serial.println("Button 1 Held");
    break;
  default:
    break;
  }
  Button::EventType button2Event = button2.checkButton();
  switch (button2Event) {
  case (Button::EventType::SingleClick):
    Serial.println("Button 2 SingleClick");
    break;
  case (Button::EventType::DoubleClick):
    Serial.println("Button 2 DoubleClick");
    break;
  case (Button::EventType::LongClick):
    Serial.println("Button 2 LongClick");
    break;
  case (Button::EventType::Held):
    Serial.println("Button 2 Held");
    break;
  default:
    break;
  }
  Button::EventType button3Event = button3.checkButton();
  switch (button3Event) {
  case (Button::EventType::SingleClick):
    Serial.println("Button 3 SingleClick");
    break;
  case (Button::EventType::DoubleClick):
    Serial.println("Button 3 DoubleClick");
    break;
  case (Button::EventType::LongClick):
    Serial.println("Button 3 LongClick");
    break;
  case (Button::EventType::Held):
    Serial.println("Button 3 Held");
    break;
  default:
    break;
  }
}
