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

#include "Throttle.h"

Throttle::Throttle(UserConfirmationInterface *confirmer, UserSelectionInterface *selector, uint8_t throttleStep,
                   uint8_t throttleStepFaster, uint8_t throttleStepFastest)
    : _confirmer(confirmer), _selector(selector), _throttleStep(throttleStep), _throttleStepFaster(throttleStepFaster),
      _throttleStepFastest(throttleStepFastest) {
  _consist = nullptr;
  _loco = nullptr;
  _speed = 0;
  _speedChanged = false;
  _direction = Direction::Forward;
  _directionChanged = false;
}

Consist *Throttle::getConsist() { return _consist; }

Loco *Throttle::getLoco() { return _loco; }

uint8_t Throttle::getSpeed() { return _speed; }

bool Throttle::speedChanged() { return _speedChanged; }

Direction Throttle::getDirection() { return _direction; }

bool Throttle::directionChanged() { return _directionChanged; }

void Throttle::handleUserConfirmationAction(UserConfirmationInterface::UserConfirmationAction action) {}

void Throttle::handleUserSelectionAction(UserSelectionInterface::UserSelectionAction action) {}
