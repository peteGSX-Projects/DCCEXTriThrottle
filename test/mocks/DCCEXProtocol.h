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

#ifndef DCCEXPROTOCOL_H
#define DCCEXPROTOCOL_H

#include "../../src/DCCEXLoco.h"
#include "../../src/DCCEXRoutes.h"
#include "../../src/DCCEXTurnouts.h"
#include "../../src/DCCEXTurntables.h"

/**
 * @brief Mock DCCEXProtocol class
 */

enum TrackPower {
  PowerOff = 0,
  PowerOn = 1,
  PowerUnknown = 2,
};

// Valid TrackManager types
enum TrackManagerMode {
  MAIN, // Normal DCC track mode
  PROG, // Programming DCC track mode
  DC,   // DC mode
  DCX,  // Reverse polarity DC mode
  NONE, // Track is unused
};

class DCCEXProtocolDelegate {
public:
  virtual void receivedServerVersion(int major, int minor, int patch) {}

  virtual void receivedMessage(char *message) {}

  virtual void receivedRosterList() {}

  virtual void receivedTurnoutList() {}

  virtual void receivedRouteList() {}

  virtual void receivedTurntableList() {}

  virtual void receivedLocoUpdate(Loco *loco) {}

  virtual void receivedLocoBroadcast(int address, int speed, Direction direction, int functionMap) {}

  virtual void receivedTrackPower(TrackPower state) {}

  virtual void receivedIndividualTrackPower(TrackPower state, int track) {}

  virtual void receivedTrackType(char track, TrackManagerMode type, int address) {}

  virtual void receivedTurnoutAction(int turnoutId, bool thrown) {}

  virtual void receivedTurntableAction(int turntableId, int position, bool moving) {}

  virtual void receivedReadLoco(int address) {}

  virtual void receivedValidateCV(int cv, int value) {}

  virtual void receivedValidateCVBit(int cv, int bit, int value) {}

  virtual void receivedWriteLoco(int address) {}

  virtual void receivedWriteCV(int cv, int value) {}

  virtual void receivedScreenUpdate(int screen, int row, char *message) {}
};

class DCCEXProtocol {
public:
  DCCEXProtocol(int maxCmdBuffer = 500, int maxCommandParams = 50) {}

  virtual void setDelegate(DCCEXProtocolDelegate *delegate) { _delegate = delegate; }

  virtual void setLogStream(Stream *console) { _console = console; }

  virtual void connect(Stream *stream) { _stream = stream; }

  virtual void getLists(bool rosterRequired, bool turnoutListRequired, bool routeListRequired,
                        bool turntableListRequired) {
    if (!_receivedLists) {
      if (rosterRequired && !_rosterRequested) {
        _rosterRequested = true;
        if (_stream)
          _stream->print("<JR>"); // Internal _getRoster()
      } else if (!rosterRequired || _receivedRoster) {
        if (turnoutListRequired && !_turnoutListRequested) {
          _turnoutListRequested = true;
          if (_stream)
            _stream->print("<JT>"); // Internal _getTurnouts()
        } else if (!turnoutListRequired || _receivedTurnoutList) {
          if (routes && !_routeListRequested) {
            _routeListRequested = true;
            if (_stream)
              _stream->print("<JA>"); // Internal _getRoutes()
          } else if (!routeListRequired || _receivedRouteList) {
            if (turntableListRequired && !_turntableListRequested) {
              _turntableListRequested = true;
              if (_stream)
                _stream->print("<JO>"); // Internal _getTurntables()
            } else if (!turntableListRequired || _receivedTurntableList) {
              _receivedLists = true;
            }
          }
        }
      }
    }
  }

  virtual bool receivedLists() {
    return (!_rosterRequested || _receivedRoster) && (!_turnoutListRequested || _receivedTurnoutList) &&
           (!_routeListRequested || _receivedRouteList) && (!_turntableListRequested || _receivedTurntableList);
  }

  virtual void check() {
    if (_rosterRequested && !_receivedRoster) {
      _receivedRoster = true;
      if (_delegate) {
        _delegate->receivedRosterList();
      }
      return;
    }

    if (_turnoutListRequested && !_receivedTurnoutList) {
      _receivedTurnoutList = true;
      if (_delegate) {
        _delegate->receivedTurnoutList();
      }
      return;
    }

    if (_routeListRequested && !_receivedRouteList) {
      _receivedRouteList = true;
      if (_delegate) {
        _delegate->receivedRouteList();
      }
      return;
    }

    if (_turntableListRequested && !_receivedTurntableList) {
      _receivedTurntableList = true;
      if (_delegate) {
        _delegate->receivedTurntableList();
      }
      return;
    }
  }

  /// @brief Linked list of Loco objects to form the roster
  Loco *roster = nullptr;

  /// @brief Linked list of Turnout objects to form the turnout list
  Turnout *turnouts = nullptr;

  /// @brief Linked list of Route objects to form the list of routes and automations
  Route *routes = nullptr;

  /// @brief Linked list of Turntable objects to form the list of turntables
  Turntable *turntables = nullptr;

  void setReceivedRoster(bool received) { _receivedRoster = received; }

  void setReceivedTurnoutList(bool received) { _receivedTurnoutList = received; }

  void setReceivedRouteList(bool received) { _receivedRouteList = received; }

  void setReceivedTurntableList(bool received) { _receivedTurntableList = received; }

  void createMockRoster() {
    Loco *rosterLoco1 = new Loco(1, LocoSource::LocoSourceRoster);
    rosterLoco1->setName("Loco 1");
    Loco *rosterLoco2 = new Loco(2, LocoSource::LocoSourceRoster);
    rosterLoco2->setName("Loco 2");
    Loco *rosterLoco3 = new Loco(3, LocoSource::LocoSourceRoster);
    rosterLoco3->setName("Loco 3");
    Loco *rosterLoco4 = new Loco(4, LocoSource::LocoSourceRoster);
    rosterLoco4->setName("Loco 4");
    Loco *rosterLoco5 = new Loco(5, LocoSource::LocoSourceRoster);
    rosterLoco5->setName("Loco 5");
    roster = Loco::getFirst();
  }

  void createMockTurnoutList() {}

  void createMockTurntableList() {}

  void createMockRouteList() {}

  virtual ~DCCEXProtocol() {
    if (roster != nullptr) {
      Loco::clearRoster();
    }

    if (turnouts != nullptr) {
      Turnout::clearTurnoutList();
    }

    if (routes != nullptr) {
      Route::clearRouteList();
    }

    if (turntables != nullptr) {
      Turntable::clearTurntableList();
    }
  }

private:
  DCCEXProtocolDelegate *_delegate = nullptr;
  Stream *_stream = nullptr;
  Stream *_console = nullptr;
  bool _receivedVersion = false;
  bool _receivedLists = false;
  bool _rosterRequested = false;
  bool _receivedRoster = false;
  bool _turnoutListRequested = false;
  bool _receivedTurnoutList = false;
  bool _routeListRequested = false;
  bool _receivedRouteList = false;
  bool _turntableListRequested = false;
  bool _receivedTurntableList = false;
};

#endif // DCCEXPROTOCOL_H
