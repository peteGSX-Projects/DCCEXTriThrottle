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

#include "CommandStationListener.h"
#include "test/mocks/MockEventListener.h"
#include <gtest/gtest.h>

using namespace testing;

/// @brief Test harness for the EventManager
class CommandStationListenerTests : public Test {
protected:
  MockEventListener *mockOrchestrator;
  EventManager *eventManager;
  CommandStationListener *commandStationListener;

  void SetUp() override {
    mockOrchestrator = new MockEventListener();
    eventManager = new EventManager();
    commandStationListener = new CommandStationListener(eventManager, nullptr);

    // Subscribe events
    eventManager->subscribe(mockOrchestrator, EventType::ReceivedTrackPower);
    eventManager->subscribe(mockOrchestrator, EventType::ReceivedLocoUpdate);
    eventManager->subscribe(mockOrchestrator, EventType::ReceivedLocoBroadcast);
  }

  void TearDown() override {
    delete commandStationListener;
    delete eventManager;
    delete mockOrchestrator;
  }
};

/**
 * @brief Test receiving a Loco object update publishes the event
 */
TEST_F(CommandStationListenerTests, TestLocoUpdateEventPublishes) {
  // Set up the expectation
  Loco *loco = new Loco(3, LocoSource::LocoSourceEntry);
  EXPECT_CALL(*mockOrchestrator,
              onEvent(AllOf(
                  // Verify the event type
                  Field(&Event::eventType, EventType::ReceivedLocoUpdate),
                  // Verify the inner data type inside EventData
                  Field(&Event::eventData, Field(&EventData::dataType, EventData::DataType::LocoData)),
                  // Verify the actual enum value inside the union
                  Field(&Event::eventData, Field(&EventData::locoValue, loco)))))
      .Times(1);

  // Simulate receiving the update
  commandStationListener->receivedLocoUpdate(loco);

  // Clean up
  delete loco;
}

/**
 * @brief Test receiving a loco broadcast update publishes the event
 */
TEST_F(CommandStationListenerTests, TestLocoBroadcastEventPublishes) {
  // Set up the expectation
  EXPECT_CALL(*mockOrchestrator,
              onEvent(AllOf(Field(&Event::eventType, EventType::ReceivedLocoBroadcast),
                            Field(&Event::eventData,
                                  AllOf(Field(&EventData::dataType, EventData::DataType::LocoBroadcastData),
                                        Field(&EventData::locoBroadcastValue,
                                              AllOf(Field(&LocoBroadcast::address, 3), Field(&LocoBroadcast::speed, 25),
                                                    Field(&LocoBroadcast::direction, Direction::Forward),
                                                    Field(&LocoBroadcast::functionMap, 0))))))))
      .Times(1);

  // Simulate receiving the update
  commandStationListener->receivedLocoBroadcast(3, 25, Direction::Forward, 0);
}

/**
 * @brief Test receiving a track power update publishes the event
 */
TEST_F(CommandStationListenerTests, TestTrackPowerEventPublishes) {
  // Set up the expectation
  EXPECT_CALL(*mockOrchestrator,
              onEvent(AllOf(
                  // 1. Verify the overall event type
                  Field(&Event::eventType, EventType::ReceivedTrackPower),

                  // 2. Verify the inner data type inside EventData
                  Field(&Event::eventData, Field(&EventData::dataType, EventData::DataType::TrackPowerData)),

                  // 3. Verify the actual enum value inside the union
                  Field(&Event::eventData, Field(&EventData::trackPowerValue, TrackPower::PowerOn)))))
      .Times(1);

  // Simulate receiving the update
  commandStationListener->receivedTrackPower(TrackPower::PowerOn);
}
