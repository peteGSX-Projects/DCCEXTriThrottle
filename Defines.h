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

#ifndef DEFINES_H
#define DEFINES_H

/**
 * @brief Default options for the various configuration parameters that can be overridden in myConfig.h.
 */

#if __has_include("myConfig.h")
#include "myConfig.h"
#else
#warning myConfig.h not found, using defaults.
#endif

// Define the default rotary encoder pins
#ifndef ENCODER1_DT
#define ENCODER1_DT PC14
#endif // ENCODER1_DT
#ifndef ENCODER1_CLK
#define ENCODER1_CLK PC15
#endif // ENCODER1_CLK
#ifndef ENCODER1_BUTTON
#define ENCODER1_BUTTON PA0
#endif // ENCODER1_BUTTON
#ifndef ENCODER2_DT
#define ENCODER2_DT PB0
#endif // ENCODER2_DT
#ifndef ENCODER2_CLK
#define ENCODER2_CLK PB1
#endif // ENCODER2_CLK
#ifndef ENCODER2_BUTTON
#define ENCODER2_BUTTON PA1
#endif // ENCODER2_BUTTON
#ifndef ENCODER3_DT
#if defined(ARDUINO_BLUEPILL_F103C8)
#define ENCODER3_DT PB10
#elif defined(ARDUINO_BLACKPILL_F411CE)
#define ENCODER3_DT PB12
#endif // PLATFORM
#endif // ENCODER3_DT
#ifndef ENCODER3_CLK
#if defined(ARDUINO_BLUEPILL_F103C8)
#define ENCODER3_CLK PB11
#elif defined(ARDUINO_BLACKPILL_F411CE)
#define ENCODER3_DT PB13
#endif // PLATFORM
#endif // ENCODER3_CLK
#ifndef ENCODER3_BUTTON
#define ENCODER3_BUTTON PA2
#endif // ENCODER3_BUTTON

// Define the default console and EX-CommandStation connect streams
#ifndef CONSOLE_STREAM
#define CONSOLE_STREAM Serial
#endif // CONSOLE_STREAM
#ifndef COMMANDSTATION_STREAM
#define COMMANDSTATION_STREAM Serial1
#endif // COMMANDSTATION_STREAM

// Set invert throttle true as default is a rotary encoder
#ifndef INVERT_THROTTLE
#define INVERT_THROTTLE true
#endif // INVERT_THROTTLE

// Set default throttle speed change steps (used to increment/decrement speed)
#ifndef THROTTLE_STEP
#define THROTTLE_STEP 1
#endif // THROTTLE_STEP
#ifndef THROTTLE_STEP_FASTER
#define THROTTLE_STEP_FASTER 2
#endif // THROTTLE_STEP_FASTER
#ifndef THROTTLE_STEP_FASTEST
#define THROTTLE_STEP_FASTEST 5
#endif // THROTTLE_STEP_FASTEST

// Set default thresholds to determine speed steps
#ifndef THROTTLE_STEP_FASTER_THRESHOLD
#define THROTTLE_STEP_FASTER_THRESHOLD 200
#endif // THROTTLE_STEP_FASTER_THRESHOLD
#ifndef THROTTLE_STEP_FASTEST_THRESHOLD
#define THROTTLE_STEP_FASTEST_THRESHOLD 125
#endif // THROTTLE_STEP_FASTEST_THRESHOLD

#endif // DEFINES_H
