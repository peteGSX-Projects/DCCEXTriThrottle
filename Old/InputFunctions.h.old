/*
 *  © 2023 Peter Cole
 *
 *  This file is for a serially connected throttle for a DCC-EX EX-CommandStation.
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

#ifndef INPUTFUNCTIONS_H
#define INPUTFUNCTIONS_H

#include <Arduino.h>
#include "Keypad.h"
#include "defines.h"
#include "DisplayFunctions.h"
#include "StaticMenus.h"
#include "ThrottleSetup.h"

extern Keypad keypad;

void setupKeypad();
void keypadEvent(KeypadEvent key);
void singleClick(int throttleNumber);
void doubleClick(int throttleNumber);
void longPress(int throttleNumber);

#endif
