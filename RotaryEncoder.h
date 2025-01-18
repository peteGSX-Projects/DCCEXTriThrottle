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

#ifndef ROTARYENCODER_H
#define ROTARYENCODER_H

#include <Arduino.h>

/**
 * @brief Mode for the RotaryEncoder input
 */
enum RotaryEncoderMode { FullStep, HalfStep };

/**
 * @brief Direction a rotary encoder has been rotated
 */
enum RotaryEncoderDirection { None, CW, CCW };

/**
 * @brief Class to enable rotary encoders to be utilised for user input
 * @details Credit for the initial state machine logic belongs to Ben Buxton (bb@cactii.net) in his original Rotary
 * Arduino library which I have reused here in a more modern C++ format
 */
class RotaryEncoder {
public:
  /**
   * @brief Construct a new Rotary Encoder object
   * @param dtPin Pin to connect to the rotary encoder's DT output
   * @param clkPin Pin to connect to the rotary encoder's CLK output
   * @param mode Operating mode for the rotary encoder, FullStep or HalfStep
   * @param inputMode Input mode for the rotary encoder pins (default INPUT_PULLUP)
   */
  RotaryEncoder(uint8_t dtPin, uint8_t clkPin, RotaryEncoderMode mode, byte inputMode = INPUT_PULLUP);

  /**
   * @brief Initialise input pins and set initial state
   */
  void begin();

  /**
   * @brief Check if the rotary encoder has been rotated, call as often as possible
   * @return RotaryEncoderDirection Direction of the move, or None if not changed
   */
  RotaryEncoderDirection check();

  /**
   * @brief Set the rotary encoder mode
   * @param mode FullStep or HalfStep
   */
  void setMode(RotaryEncoderMode mode);

private:
  const uint8_t _dtPin;
  const uint8_t _clkPin;
  RotaryEncoderMode _mode;
  const byte _inputMode;
  uint8_t _state;

  // State machine constants
  static constexpr uint8_t DIR_NONE = 0x00;
  static constexpr uint8_t DIR_CW = 0x10;
  static constexpr uint8_t DIR_CCW = 0x20;

  // Half step states
  static constexpr uint8_t R_START = 0x0;
  static constexpr uint8_t H_CCW_BEGIN = 0x1;
  static constexpr uint8_t H_CW_BEGIN = 0x2;
  static constexpr uint8_t H_START_M = 0x3;
  static constexpr uint8_t H_CW_BEGIN_M = 0x4;
  static constexpr uint8_t H_CCW_BEGIN_M = 0x5;

  // Additional states for full step mode
  static constexpr uint8_t F_CW_FINAL = 0x1;
  static constexpr uint8_t F_CW_BEGIN = 0x2;
  static constexpr uint8_t F_CW_NEXT = 0x3;
  static constexpr uint8_t F_CCW_BEGIN = 0x4;
  static constexpr uint8_t F_CCW_FINAL = 0x5;
  static constexpr uint8_t F_CCW_NEXT = 0x6;

  // Half step transition table
  static constexpr uint8_t _halfStepTable[6][4] = {
      // R_START (00)
      {H_START_M, H_CW_BEGIN, H_CCW_BEGIN, R_START},
      // H_CCW_BEGIN
      {H_START_M | DIR_CCW, R_START, H_CCW_BEGIN, R_START},
      // H_CW_BEGIN
      {H_START_M | DIR_CW, H_CW_BEGIN, R_START, R_START},
      // H_START_M (11)
      {H_START_M, H_CCW_BEGIN_M, H_CW_BEGIN_M, R_START},
      // H_CW_BEGIN_M
      {H_START_M, H_START_M, H_CW_BEGIN_M, R_START | DIR_CW},
      // H_CCW_BEGIN_M
      {H_START_M, H_CCW_BEGIN_M, H_START_M, R_START | DIR_CCW},
  };

  // Full step transition table
  static constexpr uint8_t _fullStepTable[7][4] = {
      // R_START
      {R_START, F_CW_BEGIN, F_CCW_BEGIN, R_START},
      // F_CW_FINAL
      {F_CW_NEXT, R_START, F_CW_FINAL, R_START | DIR_CW},
      // F_CW_BEGIN
      {F_CW_NEXT, F_CW_BEGIN, R_START, R_START},
      // F_CW_NEXT
      {F_CW_NEXT, F_CW_BEGIN, F_CW_FINAL, R_START},
      // F_CCW_BEGIN
      {F_CCW_NEXT, R_START, F_CCW_BEGIN, R_START},
      // F_CCW_FINAL
      {F_CCW_NEXT, F_CCW_FINAL, R_START, R_START | DIR_CCW},
      // F_CCW_NEXT
      {F_CCW_NEXT, F_CCW_FINAL, F_CCW_BEGIN, R_START},
  };
};

#endif // ROTARYENCODER_H
