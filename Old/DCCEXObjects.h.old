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

#ifndef DCCEXOBJECTS_H
#define DCCEXOBJECTS_H

#include <Arduino.h>
#include <DCCEXProtocol.h>
#include "DCCEXCallbacks.h"

extern DCCEXProtocol dccexProtocol;
extern DCCEXCallbacks dccexCallbacks;

void getDCCEXObjects();
void updateRoutes();
void updateTurnouts();
void updateTurntables();
void toggleTurnout();
void closeTurnout();
void throwTurnout();
void rotateTurntable();
void trackPowerOn();
void trackPowerOff();
// void setJoinTracks();
void setRosterLoco(int selectedLoco);
void addConsistLoco(int selectedLoco);
bool setLocoAddress(int throttle, int address);
void readLocoAddress();
void forgetLoco();
void showConsist();

#endif
