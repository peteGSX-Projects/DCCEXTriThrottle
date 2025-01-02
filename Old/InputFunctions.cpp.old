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

#include "InputFunctions.h"

bool keyPress = false;                    // Flag for when a key is pressed rather than held

/***********************************************************************************
Set up our keypad
***********************************************************************************/
char keys[4][3] = {
 {'1', '2', '3'},
 {'4', '5', '6'},
 {'7', '8', '9'},
 {'*', '0', '#'}
};
byte pin_rows[4] = {KEYPAD_PIN2, KEYPAD_PIN7, KEYPAD_PIN6, KEYPAD_PIN4};
byte pin_column[3] = {KEYPAD_PIN3, KEYPAD_PIN1, KEYPAD_PIN5};
Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, 4, 3);

// Function to set up the keypad object
void setupKeypad() {
  keypad.addEventListener(keypadEvent);
  keypad.setDebounceTime(KEYPAD_DEBOUNCE);
  keypad.setHoldTime(KEYPAD_HOLD);
}

// Function for a keypad event handler
void keypadEvent(KeypadEvent key) {
  switch (keypad.getState()) {
    case PRESSED:
      keyPress = true;
      break;
    case HOLD:
      keyPress = false;
      // displayKey(key, HOLD);
      if (menuSystem.isHome()) {
        handleThrottleKeys(key, PRESSED);
      } else {
        menuSystem.handleKeys(key, HOLD);
      }
      break;
    case RELEASED:
      if (keyPress == true) {
        // displayKey(key, PRESSED);
        if (menuSystem.isHome() && key!='*') {
          handleThrottleKeys(key, PRESSED);
        } else {
          menuSystem.handleKeys(key, PRESSED);
        }
      } else {
        // displayKey(key, RELEASED);
        if (menuSystem.isHome()) {
          handleThrottleKeys(key, PRESSED);
        } else {
          menuSystem.handleKeys(key, RELEASED);
        }
      }
      break;
    case IDLE:
      break;
    default:
      break;
  }
}

void singleClick(int throttleNumber) {
  auto th=throttles[throttleNumber];
  int speed=th->getSpeed();
  int address=th->getLocoAddress();
  if (speed==0 && address!=0) {
    th->setDirection((th->getDirection()==Direction::Reverse) ? Direction::Forward : Direction::Reverse);
    displayThrottleDirection(throttleNumber, th->getDirection(), menuSystem.isHome());
  } else if (speed>0 && address!=0) {
    th->setSpeed(0);
  }
}

void doubleClick(int throttleNumber) {

}

void longPress(int throttleNumber) {

}
