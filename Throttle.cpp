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

#include "Throttle.h"

Throttle::Throttle(int index, UserConfirmationInterface *confirmer, UserSelectionInterface *selector,
                   DCCEXProtocol *commandStationClient, Logger *logger, uint8_t throttleStep,
                   uint8_t throttleStepFaster, uint8_t throttleStepFastest)
    : _index(index), _confirmer(confirmer), _selector(selector), _commandStationClient(commandStationClient),
      _logger(logger), _throttleStep(throttleStep), _throttleStepFaster(throttleStepFaster),
      _throttleStepFastest(throttleStepFastest) {

  _consist = nullptr;
  _loco = nullptr;
  _speed = 0;
  _speedChanged = false;
  _direction = Direction::Forward;
  _directionChanged = false;
  _locoChanged = false;
}

void Throttle::setConsist(Consist *consist) {
  _loco = nullptr;
  _consist = consist;
  _locoChanged = true;
}

Consist *Throttle::getConsist() { return _consist; }

void Throttle::setLoco(Loco *loco) {
  _consist = nullptr;
  _loco = loco;
  _locoChanged = true;
}

Loco *Throttle::getLoco() { return _loco; }

uint8_t Throttle::getSpeed() { return _speed; }

bool Throttle::isSpeedPending() {
  if (!_loco && !_consist)
    return false;

  uint8_t realSpeed = (_loco != nullptr) ? _loco->getSpeed() : _consist->getSpeed();
  return _speed != realSpeed;
}

bool Throttle::speedChanged() { return _speedChanged; }

Direction Throttle::getDirection() { return _direction; }

bool Throttle::directionChanged() { return _directionChanged; }

bool Throttle::locoChanged() { return _locoChanged; }

void Throttle::HandleUserInputAction(UserInputInterface::UserInputAction action) {}

void Throttle::update() {
  UserConfirmationInterface::UserConfirmationAction confirm = _confirmer->check();
  _handleUserConfirmationAction(confirm);
  UserSelectionInterface::UserSelectionAction select = _selector->check();
  _handleUserSelectionAction(select);
}

Throttle::~Throttle() {}

void Throttle::_handleUserConfirmationAction(UserConfirmationInterface::UserConfirmationAction action) {
  if (action != UserConfirmationInterface::UserConfirmationAction::None) {
    LOG(LogLevel::LOG_DEBUG, "Throttle(%d)::UserConfirmationAction(): %d", _index, action);
  }
}

void Throttle::_handleUserSelectionAction(UserSelectionInterface::UserSelectionAction action) {
  if (action != UserSelectionInterface::UserSelectionAction::None) {
    LOG(LogLevel::LOG_DEBUG, "Throttle(%d)::UserSelectionAction(): %d", _index, action);
  }
}
