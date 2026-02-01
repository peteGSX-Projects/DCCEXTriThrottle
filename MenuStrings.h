/*
 *  © 2026 Peter Cole
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

#ifndef MENUSTRINGS_H
#define MENUSTRINGS_H

#include <Arduino.h>

// PROGMEM macro definition for native test environment
#ifndef PROGMEM
#define PROGMEM
#endif

/**
 * @brief PROGMEM string constants for menu items.
 * These strings are stored in Flash memory instead of SRAM, reducing
 * SRAM consumption significantly. All static menu labels should be
 * referenced from here.
 */

// Main menu
extern const char MENU_STR_MAIN[] PROGMEM;
extern const char MENU_STR_THROTTLE[] PROGMEM;
extern const char MENU_STR_ROSTER[] PROGMEM;
extern const char MENU_STR_TURNOUTS[] PROGMEM;
extern const char MENU_STR_TURNTABLES[] PROGMEM;
extern const char MENU_STR_ROUTES[] PROGMEM;
extern const char MENU_STR_AUTOMATIONS[] PROGMEM;
extern const char MENU_STR_TRACKS[] PROGMEM;
extern const char MENU_STR_SYSTEM[] PROGMEM;

// Throttle menu items
extern const char MENU_STR_SELECT_LOCO[] PROGMEM;
extern const char MENU_STR_ENTER_ADDRESS[] PROGMEM;

// Tracks menu items
extern const char MENU_STR_TOGGLE_POWER[] PROGMEM;

#endif // MENUSTRINGS_H
