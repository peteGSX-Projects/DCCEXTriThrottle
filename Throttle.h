/*
 *  © 2026 Peter Cole
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

#ifndef THROTTLE_H
#define THROTTLE_H

#include "Logger.h"
#include "UserConfirmationInterface.h"
#include "UserInputInterface.h"
#include "UserSelectionInterface.h"
#include <DCCEXProtocol.h>

/**
 * @brief This class enables interactive control of the selected Loco both when the ThrottleScreen is active, and when
 * the user is interacting with menus or other functions
 */
class Throttle {
public:
  /**
   * @brief Construct a new Throttle object
   * param index Index of this Throttle instance
   * @param confirmer Pointer to a class instance extending the UserConfirmationInterface class
   * @param selector Pointer to a class instance extending the UserSelectionInterface class
   * @param throttleStep Number to change the speed by for a normal speed change - UserSelectionAction::Up|Down
   * @param throttleStepFaster Number to change the speed by for a faster speed change -
   * UserSelectionAction::UpFaster|DownFaster
   * @param throttleStepFastest Number to change the speed by for the fastest speed change -
   * UserSelectionAction::UpFastest|DownFastest
   */
  Throttle(int index, UserConfirmationInterface *confirmer, UserSelectionInterface *selector, uint8_t throttleStep,
           uint8_t throttleStepFaster, uint8_t throttleStepFastest);

  /**
   * @brief Set the Consist object
   * @param consist Pointer to the Consist object
   */
  void setConsist(Consist *consist);

  /**
   * @brief Get the Consist object
   * @return Consist* Get the current Consist operated by this throttle, should be nullptr if a Loco is set
   */
  Consist *getConsist();

  /**
   * @brief Set the Loco object
   * @param loco Pointer to the Loco object
   */
  void setLoco(Loco *loco);

  /**
   * @brief Get the Loco object
   * @return Loco* Get the current Loco operated by this throttle, should be nullptr if a Consist is set
   */
  Loco *getLoco();

  /**
   * @brief Get the Speed object
   * @return uint8_t Get the current speed of this throttle
   */
  uint8_t getSpeed();

  /**
   * @brief Check if the current user selected speed is different to the Loco object's speed
   * @return true If user selected speed does not match Loco object
   * @return false If both speeds match
   */
  bool isSpeedPending();

  /**
   * @brief Check if the speed has changed
   * @return boolean
   */
  bool speedChanged();

  /**
   * @brief Get the Direction object
   * @return Direction Get the current direction of this throttle
   */
  Direction getDirection();

  /**
   * @brief Check if the direction has changed
   * @return boolean
   */
  bool directionChanged();

  /**
   * @brief Check if the associated loco has changed
   * @return true
   * @return false
   */
  bool locoChanged();

  /**
   * @brief Handle user input interactions for this Throttle instance
   * @param action Valid UserInputInterface::UserInputAction
   */
  void HandleUserInputAction(UserInputInterface::UserInputAction action);

  /**
   * @brief Call this method at least once per main loop iteration to process user interaction
   */
  void update();

  /**
   * @brief Set the Logger object
   * @param logger Pointer to the logger instance
   */
  void setLogger(Logger *logger);

  /**
   * @brief Destroy the Throttle object
   */
  ~Throttle();

private:
  int _index;
  UserConfirmationInterface *_confirmer;
  UserSelectionInterface *_selector;
  uint8_t _throttleStep;
  uint8_t _throttleStepFaster;
  uint8_t _throttleStepFastest;
  Consist *_consist;
  Loco *_loco;
  uint8_t _speed;
  bool _speedChanged;
  Direction _direction;
  bool _directionChanged;
  Logger *_logger;
  bool _locoChanged;

  /**
   * @brief Handle user confirmation interactions destined for this Throttle instance
   * @param action Valid UserConfirmationInterface::UserConfirmationAction
   */
  void _handleUserConfirmationAction(UserConfirmationInterface::UserConfirmationAction action);

  /**
   * @brief Handle user selection interactions destined for this Throttle instance
   * @param action Valid UserSelectionInterface::UserSelectionAction
   */
  void _handleUserSelectionAction(UserSelectionInterface::UserSelectionAction action);
};

#endif // THROTTLE_H
