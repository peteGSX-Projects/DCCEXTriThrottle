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

#ifndef NATIVE_TESTING

#include "RotaryEncoder.h"

RotaryEncoder::RotaryEncoder(uint8_t dtPin, uint8_t clkPin, RotaryEncoder::Mode mode, byte inputMode)
    : _dtPin(dtPin), _clkPin(clkPin), _mode(mode), _inputMode(inputMode), _state(R_START) {}

void RotaryEncoder::begin() {
  pinMode(_clkPin, _inputMode);
  pinMode(_dtPin, _inputMode);
  _state = R_START;
}

RotaryEncoder::Direction RotaryEncoder::check() {
  uint8_t pinState = (digitalRead(_clkPin) << 1) | digitalRead(_dtPin); // Read the current pin states

  // Update state and get direction in one operation
  const uint8_t result = (_mode == RotaryEncoder::Mode::HalfStep) ? _halfStepTable[_state & 0xf][pinState]
                                                                  : _fullStepTable[_state & 0xf][pinState];

  _state = result & 0xf; // Update state

  const uint8_t direction = result & 0x30; // Check direction bits

  // Return direction based on the state bits
  switch (direction) {
  case DIR_CW:
    return RotaryEncoder::Direction::CW;
  case DIR_CCW:
    return RotaryEncoder::Direction::CCW;
  default:
    return RotaryEncoder::Direction::None;
  }
}

void RotaryEncoder::setMode(RotaryEncoder::Mode mode) {
  _mode = mode;
  _state = R_START; // Reset to R_START
}

#endif // NATIVE_TESTING
