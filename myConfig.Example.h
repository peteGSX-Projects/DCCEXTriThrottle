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
 * @brief Enable testing of the physical input and display devices
 * @details To test that the various hardware devices are connected and functioning correctly, uncomment the
 * DEVICE_TESTING line below, along with one or more of the devices to be tested.
 *
 * TEST_DISPLAY will simply display some text on screen using all defined fonts, and if you see them the test is
 * successful.
 *
 * The input method tests are all prompted within the serial console and do not use the display.
 *
 * TEST_KEYPAD will prompt to press certain keys in certain ways and will report success or failure in the serial
 * console.
 *
 * TEST_ENCODERS will prompt to rotate each rotary encoder in turn a specific number of steps and will report
 * success or failure in the serial console.
 *
 * TEST_BUTTONS will prompt to press each button in certain ways and will
 * report success or failure in the serial console.
 */

// #define DEVICE_TESTING
// #define TEST_DISPLAY
// #define TEST_KEYPAD
// #define TEST_ENCODERS
// #define TEST_BUTTONS
