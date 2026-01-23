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

  _syncSpeed(select);
}

Throttle::~Throttle() {}

void Throttle::_handleUserConfirmationAction(UserConfirmationInterface::UserConfirmationAction action) {
  if (!_loco && !_consist)
    return;

  if (action != UserConfirmationInterface::UserConfirmationAction::None) {
    LOG(LogLevel::LOG_DEBUG, "Throttle(%d)::UserConfirmationAction(): %d", _index, action);

    switch (action) {
    case UserConfirmationInterface::UserConfirmationAction::SingleClick: {
      if (_speed > 0) {
        LOG(LogLevel::LOG_DEBUG, "Throttle(%d) Encoder single click while loco moving, stopping loco", _index);
        _speed = 0;
        _speedChanged = true;
      } else {
        LOG(LogLevel::LOG_DEBUG, "Throttle(%d) Encoder single click while loco stopped, changing direction", _index);
        if (_direction == Direction::Forward) {
          _direction = Direction::Reverse;
        } else {
          _direction = Direction::Forward;
        }
        _directionChanged = true;
      }
      _setThrottle();
      break;
    }
    case UserConfirmationInterface::UserConfirmationAction::LongClick: {
      LOG(LogLevel::LOG_DEBUG, "Throttle(%d) EStop", _index);
      _speed = -1;
      _speedChanged = true;
      _setThrottle();
      break;
    }
    default: {
      break;
    }
    }
  }
}

void Throttle::_handleUserSelectionAction(UserSelectionInterface::UserSelectionAction action) {
  if (!_loco && !_consist)
    return;

  if (action != UserSelectionInterface::UserSelectionAction::None) {
    LOG(LogLevel::LOG_DEBUG, "Throttle(%d)::UserSelectionAction(): %d", _index, action);
    int step = 0;
    bool increase = true;

    switch (action) {
    case UserSelectionInterface::UserSelectionAction::Up: {
      step = _throttleStep;
      break;
    }
    case UserSelectionInterface::UserSelectionAction::UpFaster: {
      step = _throttleStepFaster;
      break;
    }
    case UserSelectionInterface::UserSelectionAction::UpFastest: {
      step = _throttleStepFastest;
      break;
    }
    case UserSelectionInterface::UserSelectionAction::Down: {
      step = _throttleStep;
      increase = false;
      break;
    }
    case UserSelectionInterface::UserSelectionAction::DownFaster: {
      step = _throttleStepFaster;
      increase = false;
      break;
    }
    case UserSelectionInterface::UserSelectionAction::DownFastest: {
      step = _throttleStepFastest;
      increase = false;
      break;
    }
    default: {
      LOG(LogLevel::LOG_DEBUG, "Unknown UserSelectionAction %d", action);
      break;
    }
    }

    int newSpeed = _speed;
    if (increase) {
      newSpeed += step;
    } else {
      newSpeed -= step;
    }

    if (newSpeed > 126)
      newSpeed = 126;
    if (newSpeed < 0)
      newSpeed = 0;

    if (_speed != newSpeed) {
      _speed = newSpeed;
      _speedChanged = true;
      _setThrottle();
    }
  }
}

void Throttle::_setThrottle() {
  if (_loco) {
    _commandStationClient->setThrottle(_loco, _speed, _direction);
  } else if (_consist) {
    _commandStationClient->setThrottle(_consist, _speed, _direction);
  }
}

void Throttle::_syncSpeed(UserSelectionInterface::UserSelectionAction action) {
  if (!_loco && !_consist)
    return;

  int realSpeed = 0;
  if (_loco) {
    realSpeed = _loco->getSpeed();
  } else {
    realSpeed = _consist->getSpeed();
  }

  if (action == UserSelectionInterface::UserSelectionAction::None && _speed != realSpeed) {
    if (millis() - _lastUserInteraction > _SYNC_TIME) {
      _speed = realSpeed;
      _speedChanged = true;
    }
  }
}
