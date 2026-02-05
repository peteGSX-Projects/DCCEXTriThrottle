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
  _lastUserInteraction = 0;
}

void Throttle::setConsist(Consist *consist) {
  if (!consist)
    return;

  _loco = nullptr;
  _consist = consist;
  _locoChanged = true;
  _speed = _consist->getSpeed();
  _direction = _consist->getDirection();
  _lastUserInteraction = 0;
}

Consist *Throttle::getConsist() { return _consist; }

void Throttle::setLoco(Loco *loco) {
  if (!loco)
    return;

  _consist = nullptr;
  _loco = loco;
  _locoChanged = true;
  _speed = _loco->getSpeed();
  _direction = _loco->getDirection();
  _lastUserInteraction = 0;
}

Loco *Throttle::getLoco() { return _loco; }

void Throttle::forgetLoco() {
  if (_loco == nullptr && _consist == nullptr)
    return;

  int speed = _loco ? _loco->getSpeed() : _consist->getSpeed();
  if (speed > 0)
    return;

  if (_loco != nullptr) {
    if (_loco->getSource() == LocoSource::LocoSourceEntry) {
      delete _loco;
    }
    _loco = nullptr;
    _locoChanged = true;
  } else {
    delete _consist;
    _consist = nullptr;
    _locoChanged = true;
  }
}

int Throttle::getSpeed() { return _speed; }

bool Throttle::isSpeedPending() {
  if (!_loco && !_consist)
    return false;

  int realSpeed = (_loco != nullptr) ? _loco->getSpeed() : _consist->getSpeed();
  return _speed != realSpeed;
}

bool Throttle::speedChanged() { return _speedChanged; }

void Throttle::resetSpeedChanged() { _speedChanged = false; }

Direction Throttle::getDirection() { return _direction; }

bool Throttle::directionChanged() { return _directionChanged; }

void Throttle::resetDirectionChanged() { _directionChanged = false; }

bool Throttle::locoChanged() { return _locoChanged; }

void Throttle::resetLocoChanged() { _locoChanged = false; }

void Throttle::HandleUserInputAction(UserInputInterface::UserInputAction action) {}

void Throttle::update() {
  UserConfirmationInterface::UserConfirmationAction confirm = _confirmer->check();
  UserSelectionInterface::UserSelectionAction select = _selector->check();

  if (confirm != UserConfirmationInterface::UserConfirmationAction::None ||
      select != UserSelectionInterface::UserSelectionAction::None) {
    _lastUserInteraction = millis();
  }

  _handleUserConfirmationAction(confirm);
  _handleUserSelectionAction(select);

  _sync(select);
}

Throttle::~Throttle() {}

void Throttle::_handleUserConfirmationAction(UserConfirmationInterface::UserConfirmationAction action) {
  if ((!_loco && !_consist) || action == UserConfirmationInterface::UserConfirmationAction::None)
    return;

  LOG(LogLevel::LOG_DEBUG, "Throttle()::UserConfirmationAction(): ", (int)action);

  if (action == UserConfirmationInterface::UserConfirmationAction::SingleClick) {
    if (_speed > 0) {
      _speed = 0;
      _speedChanged = true;
    } else {
      _direction = (_direction == Direction::Forward) ? Direction::Reverse : Direction::Forward;
      _directionChanged = true;
    }
  } else if (action == UserConfirmationInterface::UserConfirmationAction::LongClick) {
    _speed = -1;
    _speedChanged = true;
  }

  if (_speedChanged || _directionChanged) {
    _setThrottle();
  }
}

void Throttle::_handleUserSelectionAction(UserSelectionInterface::UserSelectionAction action) {
  if ((!_loco && !_consist) || action == UserSelectionInterface::UserSelectionAction::None)
    return;

  LOG(LogLevel::LOG_DEBUG, "Throttle()::UserSelectionAction(): ", (int)action);

  // Use enum mapping to values to save Flash rather than switch/case
  // Make action 0 indexed so 0 - 2 are up, 3 - 5 are down
  int actionIndex = (int)action - 1;
  bool increase = (actionIndex < 3);

  // Lookup table for the steps
  static const uint8_t steps[] = {_throttleStep, _throttleStepFaster, _throttleStepFastest};

  // Use modulo 3 to pick the correct step size
  int step = steps[actionIndex % 3];

  // Calculate speed based on step and if increased
  int newSpeed = _speed + (increase ? step : -step);

  // Constrain to DCC limits
  if (newSpeed > 126)
    newSpeed = 126;
  if (newSpeed < 0)
    newSpeed = 0;

  // If changed, set it, flag it, and send the change
  if (_speed != newSpeed) {
    _speed = newSpeed;
    _speedChanged = true;
    _setThrottle();
  }
}

void Throttle::_setThrottle() {
  if (_loco) {
    _commandStationClient->setThrottle(_loco, _speed, _direction);
  } else if (_consist) {
    _commandStationClient->setThrottle(_consist, _speed, _direction);
  }
}

void Throttle::_sync(UserSelectionInterface::UserSelectionAction action) {
  if (!_loco && !_consist)
    return;

  int realSpeed = _loco ? _loco->getSpeed() : _consist->getSpeed();
  Direction realDirection = _loco ? _loco->getDirection() : _consist->getDirection();
  if (millis() - _lastUserInteraction > _SYNC_TIME) {
    if (_direction != realDirection) {
      _direction = realDirection;
      _directionChanged = true;
    }

    if (action == UserSelectionInterface::UserSelectionAction::None && _speed != realSpeed) {
      _speed = realSpeed;
      _speedChanged = true;
    }
  }
}
