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

#include <Arduino.h>
#include "defines.h"
#include "DCCEXCallbacks.h"
#include "DisplayFunctions.h"
#include "StaticMenus.h"

void DCCEXCallbacks::receivedServerDescription(String uc, String version) {
  CONSOLE.print(uc);
  CONSOLE.print(F(" running version "));
  CONSOLE.println(version);
}

/*
void DCCEXCallbacks::receivedSpeed(int throttleNo, int speed) { 
  CONSOLE.print("Received Throttle Speed: ");
  CONSOLE.print(throttleNo);
  CONSOLE.print(" Speed: ");
  CONSOLE.println(speed); 
}

void DCCEXCallbacks::receivedDirection(int throttleNo, Direction dir) { 
  CONSOLE.print("Received Throttle Direction: ");
  CONSOLE.print(throttleNo);
  CONSOLE.print(" Direction: ");
  CONSOLE.println(dir); 
}

void DCCEXCallbacks::receivedFunction(int throttleNo, int func, bool state) { 
  CONSOLE.print("Received Throttle Function change: ");
  CONSOLE.print(throttleNo);
  CONSOLE.print(" function: ");
  CONSOLE.print(func);
  CONSOLE.print(" state: ");
  CONSOLE.println(state);
}
*/

void DCCEXCallbacks::receivedLocoUpdate(Loco* loco) {
  // Need to find which throttle instance has the loco (if any)
  // Need to flag speed/direction/function change for that throttle
  Throttle* throttle=Throttle::findThrottleByLoco(throttles, NUM_THROTTLES, loco);
  if (throttle) {
    int speed=loco->getSpeed();
    Direction direction=loco->getDirection();
    int address=loco->getAddress();
    if (throttle->getLocoAddress()==address) {
      displayThrottleSpeed(throttle->getThrottleNumber(), speed, menuSystem.isHome());
      displayThrottleDirection(throttle->getThrottleNumber(), direction, menuSystem.isHome());
    }
  }
}

void DCCEXCallbacks::receivedTurnoutAction(int turnoutId, bool thrown) {
  displayTurnoutStates();
}

void DCCEXCallbacks::receivedTurntableAction(int turntableId, int position, bool moving){ 
  displayTurntableIndex();
}

void DCCEXCallbacks::receivedTrackPower(TrackPower state) {
  menuSystem.updatePowerState(state);
}

void DCCEXCallbacks::receivedReadLoco(int address) {
  CONSOLE.print("Received read loco response: ");
  CONSOLE.println(address);
}
