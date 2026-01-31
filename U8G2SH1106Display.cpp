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
  _throttleCoordinates = new ThrottleCoordinates[_numThrottles];
  _throttleCoordinates[0].speed.x = 4;
  _throttleCoordinates[0].speed.y = 20;
  _throttleCoordinates[0].direction.x = 10;
  _throttleCoordinates[0].direction.y = 35;
  _throttleCoordinates[0].address.x = 0;
  _throttleCoordinates[0].address.y = 50;
  _throttleCoordinates[1].speed.x = 46;
  _throttleCoordinates[1].speed.y = 20;
  _throttleCoordinates[1].direction.x = 52;
  _throttleCoordinates[1].direction.y = 35;
  _throttleCoordinates[1].address.x = 43;
  _throttleCoordinates[1].address.y = 50;
  _throttleCoordinates[2].speed.x = 88;
  _throttleCoordinates[2].speed.y = 20;
  _throttleCoordinates[2].direction.x = 94;
  _throttleCoordinates[2].direction.y = 35;
  _throttleCoordinates[2].address.x = 87;
  _throttleCoordinates[2].address.y = 50;
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
      _displayThrottleAddress(throttleIndex, throttle->getConsist()->getFirst()->getLoco()->getAddress(), true);
    } else {
      _displayThrottleAddress(throttleIndex, 0, false);
    }
  }
}

void U8G2SH1106Display::updateThrottleTrackPower(TrackPower state) {
  _oled->setFont(MENU_FONT);
  _oled->setCursor(113, 63);
  _oled->print("   ");
  _oled->setCursor(113, 63);
  if (state == PowerOn) {
    _oled->print("On");
  } else if (state == PowerOff) {
    _oled->print("Off");
  } else {
    _oled->print("?");
  }
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
  _oled->setFont(STATUS_FONT); // Use smaller font for status bar

  // Left: Back hint
  _oled->setCursor(0, 63);
  _oled->print("* Back");

  // Right: Pagination (only show if there's more than one page)
  if (menu->getTotalPages() > 1) {
    _oled->setCursor(70, 63);
    _oled->print("# Page ");
    _oled->print(menu->getCurrentPage() + 1);
    _oled->print("/");
    _oled->print(menu->getTotalPages());
  }

  _oled->sendBuffer();
}

void U8G2SH1106Display::displayConnectionErrorScreen() {
  _oled->clear();
  _displayHeader("Connection error");
  _displayProgressMessage("* for demo");
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

U8G2SH1106Display::~U8G2SH1106Display() {
  if (_throttleCoordinates != nullptr) {
    delete[] _throttleCoordinates;
    _throttleCoordinates = nullptr;
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
  _oled->setCursor(_throttleCoordinates[throttle].speed.x, _throttleCoordinates[throttle].speed.y);
  _oled->print("   ");
  _oled->setCursor(_throttleCoordinates[throttle].speed.x, _throttleCoordinates[throttle].speed.y);
  _oled->print(speed);
  _oled->sendBuffer();
}

void U8G2SH1106Display::_displayThrottleDirection(int throttle, Direction direction) {
  _oled->setFont(THROTTLE_FONT);
  _oled->setCursor(_throttleCoordinates[throttle].direction.x, _throttleCoordinates[throttle].direction.y);
  _oled->print("   ");
  _oled->setCursor(_throttleCoordinates[throttle].direction.x, _throttleCoordinates[throttle].direction.y);
  if (direction == Forward) {
    _oled->print("Fwd");
  } else {
    _oled->print("Rev");
  }
  _oled->sendBuffer();
}

void U8G2SH1106Display::_displayThrottleAddress(int throttle, int address, bool isConsist) {
  _oled->setFont(THROTTLE_FONT);
  _oled->setCursor(_throttleCoordinates[throttle].address.x, _throttleCoordinates[throttle].address.y);
  _oled->print("       ");
  _oled->setCursor(_throttleCoordinates[throttle].address.x, _throttleCoordinates[throttle].address.y);
  _oled->print(address);
  if (isConsist) {
    _oled->print("c");
  }
  _oled->sendBuffer();
}

void U8G2SH1106Display::_displayThrottleEStop(int throttle) {
  _oled->setFont(ESTOP_FONT);
  _oled->setCursor(_throttleCoordinates[throttle].speed.x, _throttleCoordinates[throttle].speed.y);
  _oled->print("ESTOP");
  _oled->sendBuffer();
}

#endif // NATIVE_TESTING
