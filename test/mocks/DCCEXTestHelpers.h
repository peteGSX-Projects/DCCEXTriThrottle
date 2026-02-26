/*
 *  © 2026 Peter Cole
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

#ifndef DCCEXTESTHELPERS_H
#define DCCEXTESTHELPERS_H

#include "AppOrchestrator.h"
#include <DCCEXProtocol.h>

class DCCEXTestHelpers {
public:
  /**
   * @brief Use this to simulate the empty list responses for getLists()
   * @param csConnection Stream for the connection
   */
  static void injectSuccessHandshakeEmptyLists(Stream &csConnection) {
    csConnection << "<jR>";
    csConnection << "<jT>";
    csConnection << "<jA>";
    csConnection << "<jO>";
  }

  static void injectRosterResponses(Stream &csConnection) {
    // Roster
    csConnection << "<jR 1 2 3 4 5>";
    csConnection << "<jR 1 \"Loco1\" \"Func0/*Func1\">";
    csConnection << "<jR 2 \"Loco2\" \"Func0/*Func1\">";
    csConnection << "<jR 3 \"Loco3\" \"Func0/*Func1\">";
    csConnection << "<jR 4 \"Loco4\" \"Func0/*Func1\">";
    csConnection << "<jR 5 \"Loco5\" \"Func0/*Func1\">";
  }

  static void injectTurnoutResponses(Stream &csConnection) {
    // Turnouts
    csConnection << "<jT 1 2 3 4 5>";
    csConnection << "<jT 1 0 \"Turnout1\">";
    csConnection << "<jT 2 0 \"Turnout2\">";
    csConnection << "<jT 3 0 \"Turnout3\">";
    csConnection << "<jT 4 0 \"Turnout4\">";
    csConnection << "<jT 5 0 \"Turnout5\">";
  }

  static void injectRouteResponses(Stream &csConnection) {
    // Routes
    csConnection << "<jA 1 2 3 4 5 6>";
    csConnection << "<jA 1 R \"Route1\">";
    csConnection << "<jA 2 R \"Route2\">";
    csConnection << "<jA 3 R \"Route3\">";
    csConnection << "<jA 4 A \"Automation1\">";
    csConnection << "<jA 5 A \"Automation2\">";
    csConnection << "<jA 6 A \"Automation3\">";
  }

  static void injectTurntableResponses(Stream &csConnection) {
    // Turntables
    csConnection << "<jO 1 2>";
    csConnection << "<jO 1 0 1 3 \"Turntable1\">";
    csConnection << "<jO 2 1 2 3 \"Turntable2\">";
  }

  static void injectTurntableIndexResponses(Stream &csConnection) {
    // Turntable indexes
    csConnection << "<jP 1 0 180 \"Home\">";
    csConnection << "<jP 1 1 10 \"TT1 Index1\">";
    csConnection << "<jP 1 2 20 \"TT1 Index2\">";
    csConnection << "<jP 2 0 180 \"Home\">";
    csConnection << "<jP 2 1 10 \"TT2 Index1\">";
    csConnection << "<jP 2 2 20 \"TT2 Index2\">";
  }

  static void processCSConnection(AppOrchestrator *appOrchestrator, Stream &csConnection) {
    // Assume appOrchestrator->begin() is already called so we're connecting at this point
    injectRosterResponses(csConnection);
    while (csConnection.available() > 0) {
      appOrchestrator->update();
    }
    injectTurnoutResponses(csConnection);
    while (csConnection.available() > 0) {
      appOrchestrator->update();
    }
    injectRouteResponses(csConnection);
    while (csConnection.available() > 0) {
      appOrchestrator->update();
    }
    injectTurntableResponses(csConnection);
    while (csConnection.available() > 0) {
      appOrchestrator->update();
    }
    injectTurntableIndexResponses(csConnection);
    while (csConnection.available() > 0) {
      appOrchestrator->update();
    }
  }

  static void createMockRoster(DCCEXProtocol *csClient) {
    Loco *rosterLoco1 = new Loco(1, LocoSource::LocoSourceRoster);
    rosterLoco1->setName("Loco1");
    Loco *rosterLoco2 = new Loco(2, LocoSource::LocoSourceRoster);
    rosterLoco2->setName("Loco2");
    Loco *rosterLoco3 = new Loco(3, LocoSource::LocoSourceRoster);
    rosterLoco3->setName("Loco3");
    Loco *rosterLoco4 = new Loco(4, LocoSource::LocoSourceRoster);
    rosterLoco4->setName("Loco4");
    Loco *rosterLoco5 = new Loco(5, LocoSource::LocoSourceRoster);
    rosterLoco5->setName("Loco5");
    csClient->roster = rosterLoco1;
  }

  static void createMockTurnoutList(DCCEXProtocol *csClient) {
    Turnout *turnout1 = new Turnout(1, false);
    turnout1->setName("Turnout1");
    Turnout *turnout2 = new Turnout(2, false);
    turnout2->setName("Turnout2");
    Turnout *turnout3 = new Turnout(3, false);
    turnout3->setName("Turnout3");
    Turnout *turnout4 = new Turnout(4, false);
    turnout4->setName("Turnout4");
    Turnout *turnout5 = new Turnout(5, false);
    turnout5->setName("Turnout5");
    csClient->turnouts = turnout1;
  }

  static void createMockTurntableList(DCCEXProtocol *csClient) {
    Turntable *turntable1 = new Turntable(1);
    turntable1->setType(TurntableType::TurntableTypeDCC);
    turntable1->setIndex(1);
    turntable1->setNumberOfIndexes(3);
    turntable1->setName("Turntable1");
    turntable1->addIndex(new TurntableIndex(1, 0, 180, "Home"));
    turntable1->addIndex(new TurntableIndex(1, 1, 10, "TT1 Index1"));
    turntable1->addIndex(new TurntableIndex(1, 2, 20, "TT1 Index2"));
    Turntable *turntable2 = new Turntable(2);
    turntable2->setType(TurntableType::TurntableTypeEXTT);
    turntable2->setIndex(2);
    turntable2->setNumberOfIndexes(3);
    turntable2->setName("Turntable2");
    turntable2->addIndex(new TurntableIndex(1, 0, 180, "Home"));
    turntable2->addIndex(new TurntableIndex(1, 1, 10, "TT2 Index1"));
    turntable2->addIndex(new TurntableIndex(1, 2, 20, "TT2 Index2"));
    csClient->turntables = turntable1;
  }

  static void createMockRouteList(DCCEXProtocol *csClient) {
    Route *route1 = new Route(1);
    route1->setName("Route1");
    route1->setType(RouteType::RouteTypeRoute);
    Route *route2 = new Route(2);
    route2->setName("Route2");
    route2->setType(RouteType::RouteTypeRoute);
    Route *route3 = new Route(3);
    route3->setName("Route3");
    route3->setType(RouteType::RouteTypeRoute);
    Route *route4 = new Route(4);
    route4->setName("Automation1");
    route4->setType(RouteType::RouteTypeAutomation);
    Route *route5 = new Route(5);
    route5->setName("Automation2");
    route5->setType(RouteType::RouteTypeAutomation);
    Route *route6 = new Route(6);
    route6->setName("Automation3");
    route6->setType(RouteType::RouteTypeAutomation);
    csClient->routes = route1;
  }
};

#endif // DCCEXTESTHELPERS_H
