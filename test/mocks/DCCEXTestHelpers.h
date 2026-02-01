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

  /**
   * @brief Use this to simulate the full list responses for getLists()
   * @param csConnection
   */
  static void injectSuccessHandshakeFullLists(Stream &csConnection) {
    // Roster
    csConnection << "<jR 1 2 3 4 5>";
    csConnection << "<jR 1 \"Loco1\" \"Func1\">";
    csConnection << "<jR 2 \"Loco2\" \"Func2\">";
    csConnection << "<jR 3 \"Loco3\" \"Func3\">";
    csConnection << "<jR 4 \"Loco4\" \"Func4\">";
    csConnection << "<jR 5 \"Loco5\" \"Func5\">";

    // Turnouts
    csConnection << "<jT 1 2 3 4 5>";
    csConnection << "<jT 1 0 \"Turnout1\">";
    csConnection << "<jT 2 0 \"Turnout2\">";
    csConnection << "<jT 3 0 \"Turnout3\">";
    csConnection << "<jT 4 0 \"Turnout4\">";
    csConnection << "<jT 5 0 \"Turnout5\">";

    // Routes
    csConnection << "<jA1 2 3 4 5 6>";
    csConnection << "<jA 1 R \"Route1\">";
    csConnection << "<jA 2 R \"Route2\">";
    csConnection << "<jA 3 R \"Route3\">";
    csConnection << "<jA 4 A \"Automation4\">";
    csConnection << "<jA 5 A \"Automation5\">";
    csConnection << "<jA 6 A \"Automation6\">";

    // Turntables no index names due to memory leak in the way these are handled
    csConnection << "<jO>";
    csConnection << "<jO 1 0 1 3 \"Turntable1\">";
    csConnection << "<jP 1 0 180>";
    csConnection << "<jP 1 1 10>";
    csConnection << "<jP 1 2 20>";
    csConnection << "<jO 2 1 2 3 \"Turntable2\">";
    csConnection << "<jP 2 0 180>";
    csConnection << "<jP 2 1 10>";
    csConnection << "<jP 2 2 20>";
  }

  static void createMockRoster(DCCEXProtocol *csClient) {
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
    csClient->roster = rosterLoco1;
  }

  static void createMockTurnoutList(DCCEXProtocol *csClient) {
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
    csClient->turnouts = turnout1;
  }

  static void createMockTurntableList() {}

  static void createMockRouteList(DCCEXProtocol *csClient) {
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
    csClient->routes = route1;
  }
};

#endif // DCCEXTESTHELPERS_H
