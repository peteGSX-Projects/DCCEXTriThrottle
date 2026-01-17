/*
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

#include "EventManager.h"
#include "test/mocks/MockEventListener.h"
#include <DCCEXProtocol.h>
#include <gtest/gtest.h>

using namespace testing;

/// @brief Test harness for the EventManager
class EventManagerTests : public Test {
protected:
  MockEventListener *listener;
  EventManager *eventManager;

  void SetUp() override {
    listener = new MockEventListener();
    eventManager = new EventManager(nullptr);
  }

  void TearDown() override {
    delete listener;
    delete eventManager;
  }
};

/// @brief Test an EventListener can subscribe to and unsubscribe from events
TEST_F(EventManagerTests, TestSubscribeUnsubscribe) {
  // To start with, our listener should not be subscribed to
  // ReceivedLocoBroadcast events
  EXPECT_FALSE(eventManager->isSubscribed(listener, EventType::ReceivedLocoBroadcast));

  // Subscribe and validate
  eventManager->subscribe(listener, EventType::ReceivedLocoBroadcast);

  // Now, our listener should be subscribed to ReceivedLocoBroadcast events
  EXPECT_TRUE(eventManager->isSubscribed(listener, EventType::ReceivedLocoBroadcast));

  // Then, unsubscribe and validate
  eventManager->unsubscribe(listener, EventType::ReceivedLocoBroadcast);

  // Now, our listener should be subscribed to ReceivedLocoBroadcast events
  EXPECT_FALSE(eventManager->isSubscribed(listener, EventType::ReceivedLocoBroadcast));
}

/// @brief Test an event with IntegerData can be published and received by a
/// Listener
TEST_F(EventManagerTests, TestIntegerData) {
  // Subscribe to ReceivedReadLoco which would typically use int
  eventManager->subscribe(listener, EventType::ReceivedReadLoco);

  // Expect int value of -1 with ReceivedReadLoco
  Event expectedEvent(EventType::ReceivedReadLoco, EventData((int)-1));
  EXPECT_CALL(*listener,
              onEvent(AllOf(Field(&Event::eventType, EventType::ReceivedReadLoco),
                            Field(&Event::eventData, Field(&EventData::dataType, EventData::DataType::IntegerData)),
                            Field(&Event::eventData, Field(&EventData::intValue, -1)))))
      .Times(1);

  // Publish a ReceivedReadLoco event
  int readLoco = -1;
  EventData data(readLoco);
  eventManager->publish(EventType::ReceivedReadLoco, data);

  // Verify and clear expectations
  Mock::VerifyAndClearExpectations(listener);
}

/// @brief Test an event with LocoData can be published and received by a
/// Listener
TEST_F(EventManagerTests, TestLocoData) {
  // Subscribe to ReceivedLocoUpdate which would use a Loco instance
  eventManager->subscribe(listener, EventType::ReceivedLocoUpdate);

  // Create a dummy Loco instance
  Loco *loco = new Loco(42, LocoSource::LocoSourceEntry);

  // Expect our dummy Loco instance with ReceivedLocoUpdate
  Event expectedEvent(EventType::ReceivedLocoUpdate, EventData(loco));
  EXPECT_CALL(*listener,
              onEvent(AllOf(Field(&Event::eventType, EventType::ReceivedLocoUpdate),
                            Field(&Event::eventData, Field(&EventData::dataType, EventData::DataType::LocoData)),
                            Field(&Event::eventData, Field(&EventData::locoValue, loco)))))
      .Times(1);

  // Publish a ReceivedLocoUpdate event
  EventData data(loco);
  eventManager->publish(EventType::ReceivedLocoUpdate, data);

  // Clean up
  delete loco;

  // Verify and clear expectations
  Mock::VerifyAndClearExpectations(listener);
}

/// @brief Test an event with NoneData can be published and received by a
/// Listener
TEST_F(EventManagerTests, TestNoneData) {
  // Subscribe to ReceivedRosterList which is a notification only with no data
  eventManager->subscribe(listener, EventType::ReceivedRosterList);

  // Expect empty data
  Event expectedEvent(EventType::ReceivedRosterList, EventData());
  EXPECT_CALL(*listener,
              onEvent(AllOf(Field(&Event::eventType, EventType::ReceivedRosterList),
                            Field(&Event::eventData, Field(&EventData::dataType, EventData::DataType::NoneData)))))
      .Times(1);

  // Publish a ReceivedRosterList event with empty data
  EventData data;
  eventManager->publish(EventType::ReceivedRosterList, data);

  // Verify and clear expectations
  Mock::VerifyAndClearExpectations(listener);
}

/// @brief Test an event with TrackPowerData can be published and received by a
/// Listener
TEST_F(EventManagerTests, TestTrackPowerData) {
  // Subscribe to ReceivedTrackPower which would use a Loco instance
  eventManager->subscribe(listener, EventType::ReceivedTrackPower);

  // Expect TrackPower value of TrackPower::PowerOn with ReceivedTrackPower
  Event expectedEvent(EventType::ReceivedTrackPower, EventData(TrackPower::PowerOn));
  EXPECT_CALL(*listener,
              onEvent(AllOf(Field(&Event::eventType, EventType::ReceivedTrackPower),
                            Field(&Event::eventData, Field(&EventData::dataType, EventData::DataType::TrackPowerData)),
                            Field(&Event::eventData, Field(&EventData::trackPowerValue, TrackPower::PowerOn)))))
      .Times(1);

  // Publish a ReceivedTrackPower event
  EventData data(TrackPower::PowerOn);
  eventManager->publish(EventType::ReceivedTrackPower, data);

  // Verify and clear expectations
  Mock::VerifyAndClearExpectations(listener);
}

/// @brief Test an event with LocoBroadcastData can be published and received by
/// a Listener
TEST_F(EventManagerTests, TestLocoBroadcastData) {
  // Subscribe the listener to all event types
  eventManager->subscribe(listener, EventType::ReceivedLocoBroadcast);

  // Expect a LocoBroadcast event type with LocoBroadcastData
  EXPECT_CALL(
      *listener,
      onEvent(AllOf(
          Field(&Event::eventType, EventType::ReceivedLocoBroadcast),
          Field(&Event::eventData, Field(&EventData::dataType, EventData::DataType::LocoBroadcastData)),
          Field(&Event::eventData, Field(&EventData::locoBroadcastValue,
                                         AllOf(Field(&LocoBroadcast::address, 3), Field(&LocoBroadcast::speed, 50),
                                               Field(&LocoBroadcast::direction, Direction::Forward),
                                               Field(&LocoBroadcast::functionMap, 2)))))))
      .Times(1);

  // Publish a ReceivedLocoBroadcast event for loco 3 moving forward at speed
  // 50, with functions 0 and 1 on (2)
  LocoBroadcast broadcast = {3, 50, Direction::Forward, 2};
  EventData broadcastData(broadcast);
  eventManager->publish(EventType::ReceivedLocoBroadcast, broadcastData);

  // Verify and clear expectations
  Mock::VerifyAndClearExpectations(listener);
}

/**
 * @brief Test an event with SelectLocoData can be published and received by a listener
 */
TEST_F(EventManagerTests, TestSelectLocoData) {
  // Subscribe the listener
  eventManager->subscribe(listener, EventType::LocoSelected);

  // Create a dummy Loco instance and set throttle instance 1
  Loco *loco = new Loco(3, LocoSource::LocoSourceEntry);
  int targetThrottle = 1;

  // Set the expectation
  EXPECT_CALL(*listener,
              onEvent(AllOf(Field(&Event::eventType, EventType::LocoSelected),
                            Field(&Event::eventData, Field(&EventData::dataType, EventData::DataType::SelectLocoData)),
                            Field(&Event::eventData, Field(&EventData::selectLocoValue,
                                                           AllOf(Field(&SelectLoco::loco, loco),
                                                                 Field(&SelectLoco::throttleIndex, targetThrottle)))))))
      .Times(1);

  // Publish and event
  EventData eventData(loco, targetThrottle);
  eventManager->publish(EventType::LocoSelected, eventData);

  // Clean up
  delete loco;

  // Verify and clear expectations
  Mock::VerifyAndClearExpectations(listener);
}
