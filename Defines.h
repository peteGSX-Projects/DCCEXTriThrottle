/*
 *  © 2026 Peter Cole
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

// Ensure rotary encoder modes have a defined value
#define FULL_STEP 0
#define HALF_STEP 1

// Give log defines their values before user config
#include "Logger.h"
#define LOG_NONE LOG_NONE
#define LOG_ERROR LOG_ERROR
#define LOG_WARN LOG_WARN
#define LOG_INFO LOG_INFO
#define LOG_DEBUG LOG_DEBUG

#if __has_include("myConfig.h")
#include "myConfig.h"
#else
#warning myConfig.h not found, using defaults.
#endif

// Define the default rotary encoder mode
#ifndef ENCODER_MODE
#define ENCODER_MODE FULL_STEP
// #define ENCODER_MODE HALF_STEP // alternative option
#endif // ENCODER_MODE

// Define the default rotary encoder pins
// ENCODER1
#ifndef ENCODER1_DT
#define ENCODER1_DT PC14
#endif // ENCODER1_DT
#ifndef ENCODER1_CLK
#define ENCODER1_CLK PC15
#endif // ENCODER1_CLK
#ifndef ENCODER1_BUTTON
#define ENCODER1_BUTTON PA0
#endif // ENCODER1_BUTTON

// ENCODER2
#ifndef ENCODER2_DT
#define ENCODER2_DT PB0
#endif // ENCODER2_DT
#ifndef ENCODER2_CLK
#define ENCODER2_CLK PB1
#endif // ENCODER2_CLK
#ifndef ENCODER2_BUTTON
#define ENCODER2_BUTTON PA1
#endif // ENCODER2_BUTTON

// ENCODER3
// DT/CLK different on Bluepill/Blackpill
#if defined(ARDUINO_BLUEPILL_F103C8)
#ifndef ENCODER3_DT
#define ENCODER3_DT PB10
#endif // ENCODER3_DT
#ifndef ENCODER3_CLK
#define ENCODER3_CLK PB11
#endif // ENCODER3_CLK
#elif defined(ARDUINO_BLACKPILL_F411CE)
#ifndef ENCODER3_DT
#define ENCODER3_DT PB12
#endif // ENCODER3_DT
#ifndef ENCODER3_CLK
#define ENCODER3_CLK PB13
#endif // ENCODER3_CLK
#endif // PLATFORM
// Button same on both
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

// Set invert throttle as false by default
#ifndef INVERT_THROTTLE
#define INVERT_THROTTLE false
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

// Set device testing if user has configured any device tests
#undef DEVICE_TESTING
#if defined(TEST_DISPLAY) || defined(TEST_KEYPAD) || defined(TEST_ENCODERS) || defined(TEST_BUTTONS)
#define DEVICE_TESTING
#endif

// Set default keypad dimensions
#ifndef KEYPAD_ROWS
#define KEYPAD_ROWS 4
#endif // KEYPAD_ROWS

#ifndef KEYPAD_COLUMNS
#define KEYPAD_COLUMNS 3
#endif // KEYPAD_COLUMNS

// Set default keypad pins (same for Bluepill/Blackpill)
#ifndef KEYPAD_ROW_PINS
#define KEYPAD_ROW_PINS PB4, PB9, PB8, PB6
#endif // KEYPAD_ROW_PINS

#ifndef KEYPAD_COLUMN_PINS
#define KEYPAD_COLUMN_PINS PB5, PB3, PB7
#endif // KEYPAD_COLUMN_PINS

// Set default key map
#ifndef KEYPAD_MAP
#define KEYPAD_MAP '1', '2', '3', '4', '5', '6', '7', '8', '9', '*', '0', '#'
#endif // KEYPAD_MAP

// Set default keypad timings
#ifndef KEYPAD_DEBOUNCE_TIME
#define KEYPAD_DEBOUNCE_TIME 20
#endif // KEYPAD_DEBOUNCE_TIME

#ifndef KEYPAD_HELD_THRESHOLD
#define KEYPAD_HELD_THRESHOLD 300
#endif // KEYPAD_HELD_THRESHOLD

// Set default log level if user has not
#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_WARN
#endif // LOG_LEVEL

// Set default command station connection retries
#ifndef CONNECT_RETRIES
#define CONNECT_RETRIES 10
#endif // CONNECT_RETRIES
#ifndef CONNECT_RETRY_DELAY
#define CONNECT_RETRY_DELAY 1000
#endif // CONNECT_RETRY_DELAY

// Set default DCCEXProtocol parameters
#ifndef MAX_CMD_BUFFER_SIZE
#define MAX_CMD_BUFFER_SIZE 500
#endif // MAX_CMD_BUFFER_SIZE
#ifndef MAX_CMD_PARAMS
#define MAX_CMD_PARAMS 50
#endif // MAX_CMD_PARAMS

// This will never really change so define throttle count here
#define NUM_THROTTLES 3

// Define lists to retrieve
#ifndef GET_ROSTER
#define GET_ROSTER true
#endif // GET_ROSTER
#ifndef GET_TURNOUTS
#define GET_TURNOUTS true
#endif // GET_TURNOUTS
#ifndef GET_ROUTES
#define GET_ROUTES true
#endif // GET_ROUTES
#ifndef GET_TURNTABLES
#define GET_TURNTABLES true
#endif // GET_TURNTABLES

#endif // DEFINES_H
