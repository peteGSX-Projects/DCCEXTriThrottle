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

#ifndef MOCKDCCEXPROTOCOL_H
#define MOCKDCCEXPROTOCOL_H

#include "DCCEXProtocol.h"
#include <gmock/gmock.h>

/**
 * @brief Google Mock implementation of the DCCEXProtocol class
 */
class MockDCCEXProtocol : public DCCEXProtocol {
public:
  // Pass library constructor parameters up to the base
  MockDCCEXProtocol(int maxCmdBuffer = 500, int maxCommandParams = 50) : DCCEXProtocol(maxCmdBuffer, maxCommandParams) {
    ON_CALL(*this, receivedLists()).WillByDefault([this]() { return this->DCCEXProtocol::receivedLists(); });
    ON_CALL(*this, getLists(testing::_, testing::_, testing::_, testing::_))
        .WillByDefault([this](bool a, bool b, bool c, bool d) { this->DCCEXProtocol::getLists(a, b, c, d); });
  }

  MOCK_METHOD(void, setDelegate, (DCCEXProtocolDelegate * delegate), (override));
  MOCK_METHOD(void, setLogStream, (Stream * console), (override));
  MOCK_METHOD(void, connect, (Stream * stream), (override));
  MOCK_METHOD(void, getLists,
              (bool rosterRequired, bool turnoutListRequired, bool routeListRequired, bool turntableListRequired),
              (override));
  MOCK_METHOD(bool, receivedLists, (), (override));
  MOCK_METHOD(void, check, (), (override));
};

/**
 * @brief Google Mock implementation of the DCCEXProtocolDelegate
 */
class MockDCCEXProtocolDelegate : public DCCEXProtocolDelegate {
public:
  MOCK_METHOD(void, receivedServerVersion, (int major, int minor, int patch), (override));
  MOCK_METHOD(void, receivedRosterList, (), (override));
  MOCK_METHOD(void, receivedLocoUpdate, (Loco * loco), (override));
  MOCK_METHOD(void, receivedTrackPower, (TrackPower state), (override));
};

#endif // MOCKDCCEXPROTOCOL_H
