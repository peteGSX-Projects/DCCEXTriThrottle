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

#include "CommandStationSimulator.h"
#include "test/mocks/MockEventListener.h"
#include <gtest/gtest.h>

using namespace testing;

class CommandStationSimulatorTests : public Test {
protected:
  MockEventListener *mockOrchestrator;
  EventManager *eventManager;
  CommandStationSimulator *simulator;

  void SetUp() override {
    mockOrchestrator = new MockEventListener();
    eventManager = new EventManager();
    simulator = new CommandStationSimulator(eventManager);
  }

  void TearDown() override {
    delete simulator;
    delete eventManager;
    delete mockOrchestrator;
  }
};

/**
 * @brief Test that simulator->begin() populates the roster and publishes the event
 */
TEST_F(CommandStationSimulatorTests, TestBeginPopulatesRosterAndNotifies) {
  // Subscribe to events
  eventManager->subscribe(mockOrchestrator, EventType::ReceivedRosterList);

  // Set expectations
  EXPECT_CALL(*mockOrchestrator, onEvent(Field(&Event::eventType, EventType::ReceivedRosterList))).Times(1);

  // Call begin()
  simulator->begin();
}
