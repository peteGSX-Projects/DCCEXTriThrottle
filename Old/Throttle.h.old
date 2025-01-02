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

#ifndef THROTTLE_H
#define THROTTLE_H

#include <Arduino.h>
#include "defines.h"
#include "DCCEXObjects.h"
#include "avdweb_Switch.h"
#include "Rotary.h"

typedef void (*ThrottleCallback_t)(int throttleNumber);

class Throttle {
public:
  /// @brief Constructor
  /// @param throttleNumber 
  /// @param potPin 
  /// @param initialLocoList 
  Throttle(int throttleNumber, Loco* loco, int dtPin, int clkPin, int swPin,
            ThrottleCallback_t singleClickCallback,
            ThrottleCallback_t doubleClickCallback,
            ThrottleCallback_t longPressCallback);

  /// @brief Process analogue input for speed
  void process();
  
  /// @brief Get the number of this throttle
  /// @return 
  int getThrottleNumber();
  
  /// @brief Set the loco address and source for this throttle
  /// @param loco Pointer to the loco object
  void setLoco(Loco* loco);

  bool isLoco();

  void setConsist(Consist* consist);

  Consist* getConsist();

  bool isConsist();
  
  /// @brief Get the loco address for this throttle
  /// @return 
  int getLocoAddress();
  
  /// @brief Check if the throttle speed has changed
  /// @return 
  bool speedChanged();

  void setSpeedChanged();

  void setSpeed(int speed);
  
  /// @brief Get the current speed of this throttle
  /// @return 
  int getSpeed();
  
  /// @brief Forget the loco associated with this throttle
  void forgetLoco();
  
  /// @brief Set the direction for this throttle
  /// @param direction 
  void setDirection(Direction direction);
  
  /// @brief Get the current direction for this throttle
  /// @return 
  Direction getDirection();

  void setDirectionChanged();

  bool directionChanged();
  
  /// @brief Check if the specified address is in use by any throttle
  /// @param throttleArray Pointer to the throttle array containing all throttle instances
  /// @param numThrottles Number of defined throttles (size of array)
  /// @param address DCC address to check for
  /// @return 
  static bool addressInUse(Throttle** throttleArray, int numThrottles, int address);

  /// @brief Retrieve the throttle associated with the provided loco object
  /// @param throttleArray 
  /// @param numThrottles 
  /// @param loco 
  /// @return 
  static Throttle* findThrottleByLoco(Throttle** throttleArray, int numThrottles, Loco* loco);

private:
  Rotary _encoder;
  Switch _button;
  bool _speedChanged=false;
  bool _directionChanged=false;
  int _throttleNumber;
  Loco* _loco;
  Consist* _consist;
  ThrottleCallback_t _singleClickCallback;
  ThrottleCallback_t _doubleClickCallback;
  ThrottleCallback_t _longPressCallback;

  static void _buttonSingleClickCallback(void* param);
  void _handleSingleClick();
  static void _buttonDoubleClickCallback(void* param);
  void _handleDoubleClick();
  static void _buttonLongPressCallback(void* param);
  void _handleLongPress();

};

#endif
