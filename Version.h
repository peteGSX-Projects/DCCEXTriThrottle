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

#ifndef VERSION_H
#define VERSION_H

#define VERSION "0.1.0"
// 0.1.0  - Rewrite from scratch using DCCEXProtocol 1.0.0, OOP, and GoogleTest
// 0.0.16 - Add consist management
// 0.0.15 - Updated roster selection methods to work with locos
// 0.0.14 - Updates to coincide with DCCEXProtocol library release readiness
//        - Add information screen with versions and free RAM
// 0.0.13 - Change throttle method from pots to encoders with buttons
// 0.0.12 - Add read loco option to set address (non-functional)
// 0.0.11 - Dynamic updates to display from callback events:
//        - Track power state updates
//        - Turnout state updates
//        - Turntable index updates
//        - Improved DCC-EX connection validation
// 0.0.10 - Convert to u8g2 library for neater display
// 0.0.9  - Menu system improvements to handle dynamic menus and items
// 0.0.8  - Fixed bug where direction was reversed
// 0.0.7  - Updates for revised library parser
// 0.0.6  - Enable control of loco speed/direction
//        - Enable configuring consists
//        - Add adjustable connection retries
// 0.0.5  - Manage track power available
// 0.0.4  - Menu becoming functional and receiving CS broadcasts
// 0.0.3  - Communicating with CS using DCCEXProtocol library
// 0.0.2  - Updated to basic throttle control using DCCEXProtocol library
// 0.0.1  - Basic throttle control with hard coded loco address and minimal functions

#endif