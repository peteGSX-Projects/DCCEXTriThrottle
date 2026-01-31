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

  virtual ~DCCEXProtocolDelegate() {}
};

class DCCEXProtocol {
public:
  DCCEXProtocol(int maxCmdBuffer = 500, int maxCommandParams = 50) {}

  virtual void setDelegate(DCCEXProtocolDelegate *delegate) { _delegate = delegate; }

  virtual void setLogStream(Stream *console) { _console = console; }

  virtual void connect(Stream *stream) { _stream = stream; }

  virtual void getLists(bool rosterRequired, bool turnoutListRequired, bool routeListRequired,
                        bool turntableListRequired) {
    if (_receivedLists || !_delegate)
      return;

    if (rosterRequired && !_receivedRoster) {
      _rosterRequested = true;
      _stream->print("<JR>");
      return;
    }

    if (_rosterRequested && !_receivedRoster) {
      return;
    }

    if (turnoutListRequired && !_receivedTurnoutList) {
      _turnoutListRequested = true;
      _stream->print("<JT>");
      return;
    }

    if (_turnoutListRequested && !_receivedTurnoutList) {
      return;
    }

    if (routeListRequired && !_receivedRouteList) {
      _routeListRequested = true;
      _stream->print("<JA>");
      return;
    }

    if (_routeListRequested && !_receivedRouteList) {
      return;
    }

    if (turntableListRequired && !_receivedTurntableList) {
      _turntableListRequested = true;
      _stream->print("<JO>");
      return;
    }

    if (_turntableListRequested && !_receivedTurntableList) {
      return;
    }

    _receivedLists = true;
  }

  virtual bool receivedLists() { return _receivedLists; }

  virtual void check() {
    if (_disconnected)
      return;

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

  virtual void setThrottle(Loco *loco, int speed, Direction direction) {}

  virtual void setThrottle(Consist *consist, int speed, Direction direction) {}

  virtual void emergencyStop() {
    if (_stream) {
      _stream->print("<!>");
    }
  }

  virtual void toggleTurnout(int turnoutId) {
    if (!_delegate)
      return;

    for (Turnout *t = turnouts->getFirst(); t; t = t->getNext()) {
      if (t->getId() == turnoutId) {
        bool thrown = t->getThrown() ? 0 : 1;
        t->setThrown(thrown);
      }
    }
  }

  virtual void powerOn() { _trackPower = TrackPower::PowerOn; }

  virtual void powerOff() { _trackPower = TrackPower::PowerOff; }

  TrackPower getTrackPower() { return _trackPower; }

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

  void createMockTurnoutList() {
    Turnout *turnout1 = new Turnout(1, false);
    turnout1->setName("Turnout 1");
    Turnout *turnout2 = new Turnout(2, false);
    turnout2->setName("Turnout 2");
    Turnout *turnout3 = new Turnout(3, false);
    turnout3->setName("Turnout 3");
    Turnout *turnout4 = new Turnout(4, false);
    turnout4->setName("Turnout 4");
    Turnout *turnout5 = new Turnout(5, false);
    turnout5->setName("Turnout 5");
    turnouts = Turnout::getFirst();
  }

  void createMockTurntableList() {}

  void createMockRouteList() {
    Route *route1 = new Route(1);
    route1->setName("Route 1");
    route1->setType(RouteType::RouteTypeRoute);
    Route *route2 = new Route(2);
    route2->setName("Route 2");
    route2->setType(RouteType::RouteTypeRoute);
    Route *route3 = new Route(3);
    route3->setName("Route 3");
    route3->setType(RouteType::RouteTypeRoute);
    Route *route4 = new Route(4);
    route4->setName("Automation 4");
    route4->setType(RouteType::RouteTypeAutomation);
    Route *route5 = new Route(5);
    route5->setName("Automation 5");
    route5->setType(RouteType::RouteTypeAutomation);
    Route *route6 = new Route(6);
    route6->setName("Automation 6");
    route6->setType(RouteType::RouteTypeAutomation);
  }

  void setDisconnected(bool disconnected) { _disconnected = disconnected; }

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
  bool _disconnected = false;
  TrackPower _trackPower = TrackPower::PowerUnknown;
};

#endif // DCCEXPROTOCOL_H
