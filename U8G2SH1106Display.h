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

#ifndef U8G2SH1106DISPLAY_H
#define U8G2SH1106DISPLAY_H

// Don't include this when testing
#ifndef NATIVE_TESTING

#include "Defines.h"
#include "DisplayInterface.h"
#include <U8g2lib.h>

// Default I2C/SPI pin definitions
#ifndef SCL_PIN
#define SCL_PIN PB6
#endif // SCL_PIN
#ifndef SDA_PIN
#define SDA_PIN PB7
#endif // SDA_PIN
#ifndef CS_PIN
#define CS_PIN PA4
#endif // CS_PIN
#ifndef DC_PIN
#define DC_PIN PA3
#endif // DC_PIN
// Reference:
// SCK - PA5
// MISO - PA6
// MOSI - PA7

// Give OLED connection types a value
#define OLED_I2C 1
#define OLED_SPI 2

// Default display definitions
#ifndef OLED_CONNECTION
#define OLED_CONNECTION OLED_SPI
#endif // OLED_CONNECTION
#ifndef OLED_ADDRESS
#define OLED_ADDRESS 0x3c
#endif // OLED_ADDRESS

// Define the correct OLED type based on connection
#if (OLED_CONNECTION == OLED_I2C)
#define OLED_TYPE U8G2_SH1106_128X64_NONAME_F_HW_I2C
#elif (OLED_CONNECTION == OLED_SPI)
#include <SPI.h>
#define OLED_TYPE U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI
#else
#error Invalid OLED connection type specific, must be OLED_I2C or OLED_SPI
#endif // OLED_CONNECTION

// Define default fonts in use for U8g2lib
#ifndef DEFAULT_FONT
#define DEFAULT_FONT u8g2_font_NokiaSmallPlain_tr
#endif // DEFAULT_FONT
#ifndef MENU_FONT
#define MENU_FONT u8g2_font_5x8_mr
#endif // MENU_FONT
#ifndef SPEED_FONT
#define SPEED_FONT u8g2_font_spleen12x24_mn
#endif // SPEED_FONT
#ifndef THROTTLE_FONT
#define THROTTLE_FONT u8g2_font_6x10_mr
#endif // THROTTLE_FONT
#ifndef STATUS_FONT
#define STATUS_FONT u8g2_font_micro_mr
#endif // STATUS_FONT
#ifndef ESTOP_FONT
#define ESTOP_FONT u8g2_font_6x12_mr
#endif // ESTOP_FONT

/**
 * @brief Structure for X/Y coordinates of individual throttle items
 */
struct ThrottleItemCoordinates {
  int x;
  int y;
};

/**
 * @brief Structure for a throttle's screen coordinates to update
 */
struct ThrottleCoordinates {
  ThrottleItemCoordinates speed;
  ThrottleItemCoordinates direction;
  ThrottleItemCoordinates address;
};

/**
 * @brief Display implementation for SH1106 based OLEDs using the U8g2 Arduino library
 */
class U8G2SH1106Display : public DisplayInterface {
public:
  /**
   * @brief Construct a new U8G2SH1106Display object
   * @param numThrottles Number of throttles to display
   */
  U8G2SH1106Display(int numThrottles);

  /**
   * @brief Call once to initialise the display instance
   */
  void begin() override;

  /**
   * @brief Call as required to clear the display entirely
   */
  void clear() override;

  /**
   * @brief Display the throttle screen
   * @param throttles Array containing pointers to the Throttle instances
   */
  void displayThrottleScreen(Throttle **throttles) override;

  /**
   * @brief Update the specified throttle's display
   * @param throttleIndex Index of the throttle to update
   * @param throttle Pointer to the Throttle instance
   * @param force Flag if all items should be updated regardless of changes
   */
  void updateThrottleScreen(int throttleIndex, Throttle *throttle, bool force = false) override;

  /**
   * @brief Update track power state on the throttle screen
   * @param state TrackPower
   */
  void updateThrottleTrackPower(TrackPower state) override;

  /**
   * @brief Display a menu
   * @param menu Menu object
   */
  void displayMenuScreen(Menu *menu) override;

  /**
   * @brief Display the connection error screen
   */
  void displayConnectionErrorScreen() override;

  /**
   * @brief Display the progress screen
   * @param title Title of the screen
   * @param message Message indicating what the progress is
   */
  void displayProgressScreen(const char *title, const char *message) override;

  /**
   * @brief Cause the progress screen to increment the progress indicator
   */
  void updateProgressScreen();

  /**
   * @brief Destroy the U8G2SH1106Display object
   */
  ~U8G2SH1106Display();

private:
  U8G2 *_oled;
  const uint8_t *_defaultFont = DEFAULT_FONT;
  const uint8_t *_menuFont = MENU_FONT;
  const uint8_t *_speedFont = SPEED_FONT;
  const uint8_t *_throttleFont = THROTTLE_FONT;
  int _numThrottles;
  ThrottleCoordinates *_throttleCoordinates;
  int _progressCounter;

  /**
   * @brief Calculate the height of the header text and line based on the menu font
   */
  uint16_t _calculateHeaderHeight();

  /**
   * @brief Display the provided header text on screen
   * @param headerText Text to be displayed
   */
  void _displayHeader(const char *headerText);

  /**
   * @brief Displays the progress message on screen
   * @param message Activity in progress
   */
  void _displayProgressMessage(const char *message);

  /**
   * @brief Displays the current speed for the associated throttle
   * @param throttle Number of the throttle to update
   * @param speed Speed to display
   */
  void _displayThrottleSpeed(int throttle, int speed);

  /**
   * @brief Displays the current direction for the associated throttle
   * @param throttle Number of the throttle to update
   * @param direction Direction to display
   */
  void _displayThrottleDirection(int throttle, Direction direction);

  /**
   * @brief Displays the current address of the Loco associated with the throttle
   * @param throttle Number of the throttle to update
   * @param address Address to display
   * @param isConsist True if a consist, false if it is a Loco
   */
  void _displayThrottleAddress(int throttle, int address, bool isConsist);

  /**
   * @brief Displays ESTOP for the specified throttle
   * @param throttle Number of the throttle to update
   */
  void _displayThrottleEStop(int throttle);
};

#endif // NATIVE_TESTING

#endif // U8G2SH1106DISPLAY_H
