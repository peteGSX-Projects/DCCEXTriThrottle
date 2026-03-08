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

U8G2SH1106Display::U8G2SH1106Display(int numThrottles) : _numThrottles(numThrottles) {
#if (OLED_CONNECTION == OLED_I2C)
  _oled = new OLED_TYPE(U8G2_R0, U8X8_PIN_NONE, SCL_PIN, SDA_PIN);
#elif (OLED_CONNECTION == OLED_SPI)
  _oled = new OLED_TYPE(U8G2_R0, CS_PIN, DC_PIN);
#else
#error Invalid OLED connection type specific, must be OLED_I2C or OLED_SPI
#endif // OLED_CONNECTION
  _progressCounter = 0;
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

void U8G2SH1106Display::displayThrottleScreen(Throttle **throttles, TrackPower state) {
  _oled->clear();
  for (int i = 0; i < _numThrottles; i++) {
    updateThrottleScreen(i, throttles[i], true);
  }
  _oled->drawHLine(0, 55, 128);
  _oled->setFont(MENU_FONT);
  _oled->drawStr(0, 63, "* Menu");
  _oled->drawStr(80, 63, "Power: ");
  updateThrottleTrackPower(state);
}

void U8G2SH1106Display::updateThrottleScreen(int throttleIndex, Throttle *throttle, bool force) {
  if (throttle->speedChanged() || force) {
    _displayThrottleSpeed(throttleIndex, throttle->getSpeed());
  }
  if (throttle->directionChanged() || force) {
    _displayThrottleDirection(throttleIndex, throttle->getDirection());
  }
  if (throttle->locoChanged() || force) {
    if (throttle->getLoco()) {
      _displayThrottleAddress(throttleIndex, throttle->getLoco()->getAddress(), false);
    } else if (throttle->getConsist()) {
      _displayThrottleAddress(throttleIndex, throttle->getConsist()->getFirstMember()->address, true);
    } else {
      _displayThrottleAddress(throttleIndex, 0, false);
    }
  }
}

void U8G2SH1106Display::updateThrottleTrackPower(TrackPower state) {
  uint16_t x = 113;
  uint16_t y = 63;

  const char *text = nullptr;
  if (state == PowerOn) {
    text = "On";
  } else if (state == PowerOff) {
    text = "Off";
  } else {
    text = "?";
  }

  uint16_t clearWidth = _oled->getStrWidth("Off");
  uint8_t fontHeight = _oled->getMaxCharHeight();
  _oled->setDrawColor(0);
  _oled->drawBox(x, y - fontHeight, clearWidth, y);
  _oled->setDrawColor(1);
  _oled->drawStr(x, y, text);
  _oled->sendBuffer();
}

void U8G2SH1106Display::displayMenuScreen(Menu *menu) {
  if (menu == nullptr)
    return;

  _oled->clear();
  _displayHeader(menu->getName());
  int x = 0;
  int y = _calculateHeaderHeight() + 8;
  _oled->setFont(MENU_FONT);

  for (int i = 0; i < menu->getItemsPerPage(); i++) {
    BaseMenuItem *item = menu->getItemByPageIndex(i);

    if (item != nullptr) {
      _oled->setCursor(x, y);
      _oled->print(i);
      _oled->print(" ");
      const char *label = item->getName();
      _oled->print(label);

      y += 9;

      if (i == 4) {
        x = 64;
        y = _calculateHeaderHeight() + 8;
      }
    }
  }

  _oled->drawHLine(0, 54, 128);

  // Left: Back hint
  _oled->setCursor(0, 63);
  _oled->print("* Back");

  // Right: Pagination (only show if there's more than one page)
  if (menu->getTotalPages() > 1) {
    _oled->setCursor(75, 63);
    _oled->print("# Page ");
    _oled->print(menu->getCurrentPage() + 1);
    _oled->print("/");
    _oled->print(menu->getTotalPages());
  }

  _oled->sendBuffer();
}

void U8G2SH1106Display::displayErrorScreen(const char *title, const char *message, bool halt) {
  _oled->clear();
  _displayHeader(title);
  _displayProgressMessage(message);
  _oled->drawHLine(0, 55, 128);
  _oled->setFont(MENU_FONT);
  _oled->setCursor(0, 63);
  if (halt) {
    _oled->print("HALTED");
  } else {
    _oled->print("* Continue");
  }
  _oled->sendBuffer();
}

void U8G2SH1106Display::displayProgressScreen(const char *title, const char *message) {
  _progressCounter = 0;
  _oled->clear();
  _displayHeader(title);
  _displayProgressMessage(message);
}

void U8G2SH1106Display::updateProgressScreen() {
  _oled->setDrawColor(1);
  _oled->setFont(_menuFont);
  uint16_t fontHeight = _oled->getMaxCharHeight();
  uint16_t fontWidth = _oled->getMaxCharWidth();
  uint16_t x = fontWidth * _progressCounter;
  uint16_t y = _calculateHeaderHeight() + (fontHeight * 3);
  _oled->drawStr(x, y, ".");
  _oled->sendBuffer();
  _progressCounter++;
}

void U8G2SH1106Display::displayUserEntryScreen(const char *title, const char *message) {
  _oled->clear();
  _displayHeader(title);
  _displayProgressMessage(message);
  uint16_t fontHeight = _oled->getMaxCharHeight();
  uint16_t fontWidth = _oled->getMaxCharWidth();
  uint16_t x = fontWidth;
  uint16_t y = _calculateHeaderHeight() + (fontHeight * 3);
  _oled->drawStr(x, y, "_____");
  _oled->sendBuffer();
}

void U8G2SH1106Display::displayUserEntryKey(char key, int count) {
  _oled->setDrawColor(1);
  _oled->setFont(_menuFont);
  uint16_t fontHeight = _oled->getMaxCharHeight();
  uint16_t fontWidth = _oled->getMaxCharWidth();
  uint16_t x = fontWidth * count;
  uint16_t y = _calculateHeaderHeight() + (fontHeight * 3);
  _oled->drawGlyph(x, y, key);
  _oled->sendBuffer();
}

void U8G2SH1106Display::displaySysInfoScreen(const char *version, const char *libVersion, int majorCSVersion,
                                             int minorCSVersion, int patchCSVersion, int bytesFree) {
  _oled->clear();
  _displayHeader("System Info");
  _oled->setFont(_menuFont);
  uint16_t fontHeight = _oled->getMaxCharHeight();
  uint16_t y = _calculateHeaderHeight() + (fontHeight);
  _oled->setCursor(0, y);
  _oled->print("Version: ");
  _oled->print(version);
  y = y + fontHeight;
  _oled->setCursor(0, y);
  _oled->print("Lib Version: ");
  _oled->print(libVersion);
  y = y + fontHeight;
  _oled->setCursor(0, y);
  _oled->print("CS Version: ");
  _oled->print(majorCSVersion);
  _oled->print(".");
  _oled->print(minorCSVersion);
  _oled->print(".");
  _oled->print(patchCSVersion);
  y = y + fontHeight;
  _oled->setCursor(0, y);
  _oled->print("Free bytes: ");
  _oled->print(bytesFree);
  _oled->drawHLine(0, 54, 128);
  _oled->setFont(STATUS_FONT); // Use smaller font for status bar
  // Left: Back hint
  _oled->setCursor(0, 63);
  _oled->print("* Back");
  _oled->sendBuffer();
}

U8G2SH1106Display::~U8G2SH1106Display() {}

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

void U8G2SH1106Display::_displayProgressMessage(const char *message) {
  _oled->setDrawColor(1);
  _oled->setFont(_menuFont);
  uint16_t fontHeight = _oled->getMaxCharHeight();
  uint16_t x = 0;
  uint16_t y = _calculateHeaderHeight() + fontHeight + 1;
  _oled->drawStr(x, y, message);
  _oled->sendBuffer();
}

void U8G2SH1106Display::_displayThrottleSpeed(int throttle, int speed) {
  _oled->setFont(SPEED_FONT);
  // Set the y coordinate for all
  uint16_t y = 20;
  // Get parameters from display instance
  uint16_t displayWidth = _oled->getWidth();
  uint8_t fontHeight = _oled->getMaxCharHeight();
  char speedBuffer[4];
  itoa(speed, speedBuffer, 10);
  uint16_t boxWidth = displayWidth / 3;

  // Calculate the x for the speed string to be centred
  uint16_t x = _calculateCentredX(throttle, _oled->getStrWidth(speedBuffer));

  _oled->setDrawColor(0);
  _oled->drawBox(throttle * boxWidth, y - fontHeight, boxWidth, fontHeight);
  _oled->setDrawColor(1);
  _oled->setCursor(x, y);
  _oled->print(speedBuffer);
  _oled->sendBuffer();
}

void U8G2SH1106Display::_displayThrottleDirection(int throttle, Direction direction) {
  _oled->setFont(THROTTLE_FONT);
  // Set y for all
  uint16_t y = 35;

  // Calculate box dimensions
  uint16_t boxWidth = _oled->getWidth() / 3;
  uint8_t fontHeight = _oled->getMaxCharHeight();

  // Get direction string and calculate x
  const char *text = (direction == Direction::Forward) ? "Fwd" : "Rev";
  uint16_t x = _calculateCentredX(throttle, _oled->getStrWidth(text));

  // Clear box and display text
  _oled->setDrawColor(0);
  _oled->drawBox(throttle * boxWidth, y - fontHeight, boxWidth, fontHeight);
  _oled->setDrawColor(1);
  _oled->setCursor(x, y);
  _oled->print(text);
  _oled->sendBuffer();
}

void U8G2SH1106Display::_displayThrottleAddress(int throttle, int address, bool isConsist) {
  _oled->setFont(THROTTLE_FONT);
  // Set y for all
  uint16_t y = 50;

  // Calculate box dimensions
  uint16_t boxWidth = _oled->getWidth() / 3;
  uint8_t fontHeight = _oled->getMaxCharHeight();

  // Get address as a string
  char addressBuffer[7]; // max 5 digits plus c for consist and null terminator
  itoa(address, addressBuffer, 10);
  if (isConsist) {
    int len = strlen(addressBuffer);
    if (len < (int)sizeof(addressBuffer) - 1) {
      addressBuffer[len] = 'c';
      addressBuffer[len + 1] = '\0';
    }
  }
  uint16_t x = _calculateCentredX(throttle, _oled->getStrWidth(addressBuffer));

  _oled->setDrawColor(0);
  _oled->drawBox(throttle * boxWidth, y - fontHeight, boxWidth, fontHeight);
  _oled->setDrawColor(1);
  _oled->setCursor(x, y);
  _oled->print(addressBuffer);
  _oled->sendBuffer();
}

uint16_t U8G2SH1106Display::_calculateCentredX(int throttle, uint16_t textWidth) {
  uint16_t boxWidth = _oled->getWidth() / 3;
  uint16_t boxX = throttle * boxWidth;
  return boxX + (boxWidth / 2) - (textWidth / 2);
}

#endif // NATIVE_TESTING
