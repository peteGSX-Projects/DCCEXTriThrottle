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

#include "ThrottleSetup.h"
#include "DisplayFunctions.h"
#include "InputFunctions.h"

// Array to hold all throttle objects to process
Throttle* throttles[NUM_THROTTLES];

ThrottleSetup throttleSetup[NUM_THROTTLES]={
  { // Throttle 0
    {4, 20},
    {10, 35},
    {0, 50},
    ENC1_DT,
    ENC1_CLK,
    BUTTON1,
  },
  { // Throttle 1
    {46, 20},
    {52, 35},
    {43, 50},
    ENC2_DT,
    ENC2_CLK,
    BUTTON2,
  },
  { // Throttle 2
    {88, 20},
    {94, 35},
    {87, 50},
    ENC3_DT,
    ENC3_CLK,
    BUTTON3,
  },
};

// Setup the correct number of throttles as defined
void setupThrottles() {
  for (int i=0; i<NUM_THROTTLES; i++) {
    throttles[i]=new Throttle(i, nullptr,
                              throttleSetup[i].dtPin,
                              throttleSetup[i].clkPin,
                              throttleSetup[i].swPin,
                              singleClick,
                              nullptr,
                              nullptr);
  }
}

void handleThrottleKeys(char key, KeyState keyState) {
  switch(key) {
        case'1': {
          auto th=throttles[0];
          int speed=th->getSpeed();
          int address=th->getLocoAddress();
          if (speed==0 && address!=0) {
            th->setDirection((th->getDirection()==Direction::Reverse) ? Direction::Forward : Direction::Reverse);
            displayThrottleDirection(0, th->getDirection(), menuSystem.isHome());
          }
          break;
        }

        if (NUM_THROTTLES>1) {
          case '2': { // Reverse key
            auto th=throttles[1];
            int speed=th->getSpeed();
            int address=th->getLocoAddress();
            if (speed==0 && address!=0) {
              th->setDirection((th->getDirection()==Direction::Reverse) ? Direction::Forward : Direction::Reverse);
              displayThrottleDirection(1, th->getDirection(), menuSystem.isHome());
            }
            break;
          }

          case '5': // Light (press)/horn (hold) key
            break;

          case '8': // Display functions key
            break;
        }
        
        if (NUM_THROTTLES>2) {
          case '3': {
            auto th=throttles[2];
            int speed=th->getSpeed();
            int address=th->getLocoAddress();
            if (speed==0 && address!=0) {
              th->setDirection((th->getDirection()==Direction::Reverse) ? Direction::Forward : Direction::Reverse);
              displayThrottleDirection(2, th->getDirection(), menuSystem.isHome());
            }
            break;
          }
        }
    default:
      break;
  }
}
