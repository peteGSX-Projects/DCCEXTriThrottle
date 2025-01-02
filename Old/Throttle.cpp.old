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

#include "Throttle.h"

// Constructor, set input mode on construction
Throttle::Throttle(int throttleNumber, Loco* loco, int dtPin, int clkPin, int swPin,
            ThrottleCallback_t singleClickCallback,
            ThrottleCallback_t doubleClickCallback,
            ThrottleCallback_t longPressCallback)
  : _encoder(dtPin, clkPin), _button(swPin),
    _singleClickCallback(singleClickCallback),
    _doubleClickCallback(doubleClickCallback),
    _longPressCallback(longPressCallback) {
  _loco=nullptr;
  _consist=nullptr;
  _throttleNumber=throttleNumber;

  _button.setSingleClickCallback(_buttonSingleClickCallback, this);
  _button.setDoubleClickCallback(_buttonDoubleClickCallback, this);
  _button.setLongPressCallback(_buttonLongPressCallback, this);
}

// Process throttle object:
void Throttle::process() {
  int speed=0;
  if (_loco) {
    speed=_loco->getSpeed();
  } else if (_consist) {
    speed=_consist->getSpeed();
  } else {
    return;
  }
  unsigned char result=_encoder.process();
  if (result==DIR_CW && speed<126) {
    speed++;
    _speedChanged=true;
  } else if (result==DIR_CCW && speed>0) {
    speed--;
    _speedChanged=true;
  } else {
    _speedChanged=false;
  }
  if (_speedChanged==true && _loco) {
    dccexProtocol.setThrottle(_loco, speed, _loco->getDirection());
  } else if (_speedChanged && _consist) {
    dccexProtocol.setThrottle(_consist, speed, _consist->getDirection());
  }
  _button.poll();

}

// Return this throttle's number (for display purposes)
int Throttle::getThrottleNumber() {
  return _throttleNumber;
}

void Throttle::setLoco(Loco* loco) {
  if (_loco && _loco->getSource()==LocoSource::LocoSourceEntry) {
    delete _loco;
  }
  _loco=loco;
}

bool Throttle::isLoco() {
  if (_loco) return true;
  return false;
}

void Throttle::setConsist(Consist* consist) {
  _consist=consist;
}

Consist* Throttle::getConsist() {
  return _consist;
}

bool Throttle::isConsist() {
  if (_consist) return true;
  return false;
}

// Return the current loco address
int Throttle::getLocoAddress() {
  if (_loco) {
    return _loco->getAddress();
  } else if (_consist) {
    return _consist->getFirst()->getLoco()->getAddress();
  } else {
    return 0;
  }
}

// Function to flag if the speed has changed
// Sends new speed to the CS also
bool Throttle::speedChanged() {
  return _speedChanged;
}

void Throttle::setSpeedChanged() {
  _speedChanged=true;
}

void Throttle::setSpeed(int speed) {
  if (_loco) {
    dccexProtocol.setThrottle(_loco, speed, _loco->getDirection());
  } else if (_consist) {
    dccexProtocol.setThrottle(_consist, speed, _consist->getDirection());
  } else {
    return;
  }
}

// Returns the current speed
int Throttle::getSpeed() {
  if (_loco) {
    return _loco->getSpeed();
  } else if (_consist) {
    return _consist->getSpeed();
  } else {
    return 0;
  }
}

// Forgets the acquired loco
// This needs to delete any Loco or Consist objects in use
void Throttle::forgetLoco() {
  if (_loco) {
    if (_loco->getSource()==LocoSource::LocoSourceEntry) {
      delete _loco;
    }
    _loco=nullptr;
  }
  if (_consist) {
    Consist* consist=_consist;
    consist->removeAllLocos();
    _consist=nullptr;
  }
}

void Throttle::setDirection(Direction direction){
  if (_loco) {
    dccexProtocol.setThrottle(_loco, _loco->getSpeed(), direction);
  } else if (_consist) {
    dccexProtocol.setThrottle(_consist, _consist->getSpeed(), direction);
  } else {
    return;
  }
}

// Get current throttle direction
Direction Throttle::getDirection() {
  if (_loco) {
    return _loco->getDirection();
  } else if (_consist) {
    return _consist->getDirection();
  } else {
    return Direction::Forward;
  }
}

void Throttle::setDirectionChanged() {
  _directionChanged=true;
}

bool Throttle::directionChanged() {
  return _directionChanged;
}

// Function to check if the specified address is in use by any throttle
bool Throttle::addressInUse(Throttle** throttleArray, int numThrottles, int address) {
  for (int i=0; i<numThrottles; i++) {
    if (!throttleArray[i]) continue;
    auto th=throttleArray[i];
    if (th->getLocoAddress()==address) return true;
    if (th->isConsist()) {
      for (ConsistLoco* cl=th->getConsist()->getFirst(); cl; cl=cl->getNext()) {
        if (cl->getLoco()->getAddress()==address) return true;
      }
    }
  }
  return false;
}

Throttle* Throttle::findThrottleByLoco(Throttle** throttleArray, int numThrottles, Loco* loco) {
  for (int i=0; i<numThrottles; ++i) {
    if (throttleArray[i]!=nullptr && throttleArray[i]->_loco == loco) {
      return throttleArray[i];
    }
  }
  return nullptr;
}

void Throttle::_buttonSingleClickCallback(void* param) {
  if (param) {
    Throttle* th=static_cast<Throttle*>(param);
    th->_handleSingleClick();
  }
}

void Throttle::_handleSingleClick() {
  if (_singleClickCallback) {
    _singleClickCallback(_throttleNumber);
  }
}

void Throttle::_buttonDoubleClickCallback(void* param) {
  if (param) {
    Throttle* th=static_cast<Throttle*>(param);
    th->_handleDoubleClick();
  }
}

void Throttle::_handleDoubleClick() {
  if (_doubleClickCallback) {
    _doubleClickCallback(_throttleNumber);
  }
}

void Throttle::_buttonLongPressCallback(void* param) {
  if (param) {
    Throttle* th=static_cast<Throttle*>(param);
    th->_handleLongPress();
  }
}

void Throttle::_handleLongPress() {
  if (_longPressCallback) {
    _longPressCallback(_throttleNumber);
  }
}
