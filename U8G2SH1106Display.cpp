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

#include "U8G2SH1106Display.h"

// Don't include this when testing
#ifndef NATIVE_TESTING

U8G2SH1106Display::U8G2SH1106Display() {
#if (OLED_CONNECTION == OLED_I2C)
  _oled = new OLED_TYPE(U8G2_R0, U8X8_PIN_NONE, SCL_PIN, SDA_PIN);
#elif (OLED_CONNECTION == OLED_SPI)
  _oled = new OLED_TYPE(U8G2_R0, CS_PIN, DC_PIN);
#else
#error Invalid OLED connection type specific, must be OLED_I2C or OLED_SPI
#endif // OLED_CONNECTION
}

void U8G2SH1106Display::begin() {
  _oled->begin();
  _oled->setFont(_defaultFont);
  _oled->clear();
  _oled->sendBuffer();
}

void U8G2SH1106Display::clear() {
  _oled->clear();
  _oled->sendBuffer();
}

void U8G2SH1106Display::displayStartupScreen(const char *headerText, const char *version) {
  if (_needsRedraw) {
    _needsRedraw = false;
    _displayHeader(headerText);
    _displayStartupInfo(version);
  }
}

uint16_t U8G2SH1106Display::_calculateHeaderHeight() {
  _oled->setFont(_menuFont);
  uint16_t fontHeight = _oled->getMaxCharHeight();
  return fontHeight + 1;
}

void U8G2SH1106Display::_displayHeader(const char *headerText) {
  _oled->setDrawColor(1);
  _oled->setFont(_menuFont);
  uint16_t x = 0;
  uint16_t y = _calculateHeaderHeight();
  uint16_t lineWidth = _oled->getWidth();
  _oled->drawHLine(x, y, lineWidth);
  _oled->drawStr(x, y - 1, headerText);
  _oled->sendBuffer();
}

void U8G2SH1106Display::_displayStartupInfo(const char *version) {
  _oled->setDrawColor(1);
  _oled->setFont(_menuFont);
  uint16_t fontHeight = _oled->getMaxCharHeight();
  uint16_t x = 0;
  uint16_t y = _calculateHeaderHeight() + fontHeight + 1;
  const char *text = "Version: ";
  uint16_t textWidth = _oled->getStrWidth(text);
  _oled->drawStr(x, y, text);
  x += textWidth;
  _oled->drawStr(x, y, version);
  y = _oled->getHeight() - 1;
  x = 0;
  _oled->drawStr(x, y, "Press button to continue");
  _oled->sendBuffer();
}

#endif // NATIVE_TESTING
