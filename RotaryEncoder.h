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

#ifndef NATIVE_TESTING

#include "Defines.h"
#include "UserSelectionInterface.h"
#include <Arduino.h>

/**
 * @brief Class to enable rotary encoders to be utilised for user input
 * @details This class provides a modern C++ implementation of Ben Buxton's (bb@cactii.net) Rotary Arduino library for
 * interfacing with rotary encoders. Full credit to Ben for the state machine logic he implemented for reliable
 * operation of these.
 *
 * This supports both half step and full step operation for both clockwise and counter-clockwise operation.
 *
 * @note Ensure you call the begin() method to initialise each instance correctly, and then call the check() method
 * frequently in your main loop for accurate detection of rotary encoder movements.
 *
 * Usage example:
 *
 * @code {.cpp}
 * #include "RotaryEncoder.h"
 *
 * RotaryEncoder *encoder = new RotaryEncoder(2, 3, RotaryEncoder::Mode::HalfStep);
 *
 * void setup() {
 *   encoder->begin();
 * }
 *
 * void loop() {
 *   RotaryEncoder::Direction direction = encoder->checkDirection();
 *
 *   if (direction == RotaryEncoder::Direction::CW) {
 *     // Handle clockwise rotation
 *   } else if (direction == RotaryEncoder::Direction::CCW) {
 *     // Handle counter-clockwise rotations
 *   }
 * }
 * @endcode
 *
 * @section credit Credit to Ben Buxton
 *
 * These are Ben's original comments from the Rotary library.
 *
 * Rotary encoder handler for arduino. v1.1
 *
 * Copyright 2011 Ben Buxton. Licenced under the GNU GPL Version 3.
 * Contact: bb@cactii.net
 *
 * A typical mechanical rotary encoder emits a two bit gray code
 * on 3 output pins. Every step in the output (often accompanied
 * by a physical 'click') generates a specific sequence of output
 * codes on the pins.
 *
 * There are 3 pins used for the rotary encoding - one common and
 * two 'bit' pins.
 *
 * The following is the typical sequence of code on the output when
 * moving from one step to the next:
 *
 *   Position   Bit1   Bit2
 *   ----------------------
 *     Step1     0      0
 *      1/4      1      0
 *      1/2      1      1
 *      3/4      0      1
 *     Step2     0      0
 *
 * From this table, we can see that when moving from one 'click' to
 * the next, there are 4 changes in the output code.
 *
 * - From an initial 0 - 0, Bit1 goes high, Bit0 stays low.
 * - Then both bits are high, halfway through the step.
 * - Then Bit1 goes low, but Bit2 stays high.
 * - Finally at the end of the step, both bits return to 0.
 *
 * Detecting the direction is easy - the table simply goes in the other
 * direction (read up instead of down).
 *
 * To decode this, we use a simple state machine. Every time the output
 * code changes, it follows state, until finally a full steps worth of
 * code is received (in the correct order). At the final 0-0, it returns
 * a value indicating a step in one direction or the other.
 *
 * It's also possible to use 'half-step' mode. This just emits an event
 * at both the 0-0 and 1-1 positions. This might be useful for some
 * encoders where you want to detect all positions.
 *
 * If an invalid state happens (for example we go from '0-1' straight
 * to '1-0'), the state machine resets to the start until 0-0 and the
 * next valid codes occur.
 *
 * The biggest advantage of using a state machine over other algorithms
 * is that this has inherent debounce built in. Other algorithms emit spurious
 * output with switch bounce, but this one will simply flip between
 * sub-states until the bounce settles, then continue along the state
 * machine.
 * A side effect of debounce is that fast rotations can cause steps to
 * be skipped. By not requiring debounce, fast rotations can be accurately
 * measured.
 * Another advantage is the ability to properly handle bad state, such
 * as due to EMI, etc.
 * It is also a lot simpler than others - a static state table and less
 * than 10 lines of logic.
 */
class RotaryEncoder : public UserSelectionInterface {
public:
  /**
   * @brief Mode for the RotaryEncoder input
   */
  enum class Mode { FullStep, HalfStep };

  /**
   * @brief Direction a rotary encoder has been rotated
   */
  enum class Direction { None, CW, CCW };

  /**
   * @brief Construct a new Rotary Encoder object
   * @param dtPin Pin to connect to the rotary encoder's DT output
   * @param clkPin Pin to connect to the rotary encoder's CLK output
   * @param mode Operating mode for the rotary encoder, FullStep or HalfStep
   * @param inputMode Input mode for the rotary encoder pins (default INPUT_PULLUP)
   */
  RotaryEncoder(uint8_t dtPin, uint8_t clkPin, RotaryEncoder::Mode mode, byte inputMode = INPUT_PULLUP);

  /**
   * @brief Initialise input pins and set initial state
   */
  void begin() override;

  /**
   * @brief Check if the rotary encoder has been rotated, call as often as possible
   * @return RotaryEncoderDirection Direction of the move, or None if not changed
   */
  RotaryEncoder::Direction checkDirection();

  /**
   * @brief Check for the current UserSelectionAction
   * @return UserSelectionAction
   */
  UserSelectionInterface::UserSelectionAction check() override;

  /**
   * @brief Set the rotary encoder mode
   * @param mode FullStep or HalfStep
   */
  void setMode(RotaryEncoder::Mode mode);

private:
  const uint8_t _dtPin;
  const uint8_t _clkPin;
  RotaryEncoder::Mode _mode;
  const byte _inputMode;
  uint8_t _state;
  unsigned long _throttleStepFasterThreshold;
  unsigned long _throttleStepFastestThreshold;
  unsigned long _lastUpThrottleStep;
  unsigned long _lastDownThrottleStep;

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
      {H_START_M, H_CW_BEGIN, H_CCW_BEGIN, R_START},            // R_START (00)
      {H_START_M | DIR_CCW, R_START, H_CCW_BEGIN, R_START},     // H_CCW_BEGIN
      {H_START_M | DIR_CW, H_CW_BEGIN, R_START, R_START},       // H_CW_BEGIN
      {H_START_M, H_CCW_BEGIN_M, H_CW_BEGIN_M, R_START},        // H_START_M (11)
      {H_START_M, H_START_M, H_CW_BEGIN_M, R_START | DIR_CW},   // H_CW_BEGIN_M
      {H_START_M, H_CCW_BEGIN_M, H_START_M, R_START | DIR_CCW}, // H_CCW_BEGIN_M
  };

  // Full step transition table
  static constexpr uint8_t _fullStepTable[7][4] = {
      {R_START, F_CW_BEGIN, F_CCW_BEGIN, R_START},           // R_START
      {F_CW_NEXT, R_START, F_CW_FINAL, R_START | DIR_CW},    // F_CW_FINAL
      {F_CW_NEXT, F_CW_BEGIN, R_START, R_START},             // F_CW_BEGIN
      {F_CW_NEXT, F_CW_BEGIN, F_CW_FINAL, R_START},          // F_CW_NEXT
      {F_CCW_NEXT, R_START, F_CCW_BEGIN, R_START},           // F_CCW_BEGIN
      {F_CCW_NEXT, F_CCW_FINAL, R_START, R_START | DIR_CCW}, // F_CCW_FINAL
      {F_CCW_NEXT, F_CCW_FINAL, F_CCW_BEGIN, R_START},       // F_CCW_NEXT
  };
};

#endif // NATIVE_TESTING

#endif // ROTARYENCODER_H
