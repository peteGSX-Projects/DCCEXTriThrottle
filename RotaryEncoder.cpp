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

#include "RotaryEncoder.h"

RotaryEncoder::RotaryEncoder(uint8_t dtPin, uint8_t clkPin, RotaryEncoderMode mode, byte inputMode)
    : _dtPin(dtPin), _clkPin(clkPin), _mode(mode), _inputMode(inputMode), _state(0) {}

void RotaryEncoder::begin() {
  pinMode(_clkPin, _inputMode);
  pinMode(_dtPin, _inputMode);
  _state = 0; // R_START
}

RotaryEncoderDirection RotaryEncoder::check() {
  uint8_t currentState = (digitalRead(_clkPin) << 1) | digitalRead(_dtPin); // Read the current pin states

  // Get the new state from the appropriate state table
  if (_mode == RotaryEncoderMode::HalfStep) {
    _state = _halfStepTable[_state & 0xf][currentState];
  } else {
    _state = _fullStepTable[_state & 0xf][currentState];
  }

  // Return direction based on the state bits
  switch (_state & 0x30) {
  case DIR_CW:
    return RotaryEncoderDirection::CW;
  case DIR_CCW:
    return RotaryEncoderDirection::CCW;
  default:
    return RotaryEncoderDirection::None;
  }
}

void RotaryEncoder::setMode(RotaryEncoderMode mode) {
  _mode = mode;
  _state = 0; // Reset to R_START
}
