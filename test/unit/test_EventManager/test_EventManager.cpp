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
  // ReceivedReadLoco events
  EXPECT_FALSE(eventManager->isSubscribed(listener, EventType::ReceivedReadLoco));

  // Subscribe and validate
  eventManager->subscribe(listener, EventType::ReceivedReadLoco);

  // Now, our listener should be subscribed to ReceivedLocoBroadcast events
  EXPECT_TRUE(eventManager->isSubscribed(listener, EventType::ReceivedReadLoco));

  // Then, unsubscribe and validate
  eventManager->unsubscribe(listener, EventType::ReceivedReadLoco);

  // Now, our listener should be subscribed to ReceivedLocoBroadcast events
  EXPECT_FALSE(eventManager->isSubscribed(listener, EventType::ReceivedReadLoco));
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

/// @brief Test an event with NoneData can be published and received by a
/// Listener
TEST_F(EventManagerTests, TestNoneData) {
  // Subscribe to ExitMenu which is a notification only with no data
  eventManager->subscribe(listener, EventType::ExitMenu);

  // Expect empty data
  Event expectedEvent(EventType::ExitMenu, EventData());
  EXPECT_CALL(*listener,
              onEvent(AllOf(Field(&Event::eventType, EventType::ExitMenu),
                            Field(&Event::eventData, Field(&EventData::dataType, EventData::DataType::NoneData)))))
      .Times(1);

  // Publish a ReceivedRosterList event with empty data
  EventData data;
  eventManager->publish(EventType::ExitMenu, data);

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

  // Publish the event
  EventData eventData(loco, targetThrottle);
  eventManager->publish(EventType::LocoSelected, eventData);

  // Clean up
  delete loco;

  // Verify and clear expectations
  Mock::VerifyAndClearExpectations(listener);
}

/**
 * @brief Test a LocoAddressEntered event can be published and received by a listener
 */
TEST_F(EventManagerTests, TestEnterLocoAddressData) {
  // Subscribe the listener
  eventManager->subscribe(listener, EventType::LocoAddressEntered);

  // Set the parameters
  int address = 1234;
  int throttleIndex = 1;

  // Set the expectation
  EXPECT_CALL(*listener,
              onEvent(AllOf(Field(&Event::eventType, EventType::LocoAddressEntered),
                            Field(&Event::eventData, Field(&EventData::dataType, EventData::DataType::LocoAddressData)),
                            Field(&Event::eventData, Field(&EventData::locoAddressValue,
                                                           AllOf(Field(&LocoAddress::address, address),
                                                                 Field(&LocoAddress::throttleIndex, throttleIndex)))))))
      .Times(1);

  // Publish the event
  EventData eventData(address, throttleIndex);
  eventManager->publish(EventType::LocoAddressEntered, eventData);

  // Verify and clear expectations
  Mock::VerifyAndClearExpectations(listener);
}

/**
 * @brief Test a RequestStateChange event can be published and received by the listener
 */
TEST_F(EventManagerTests, TestRequestStateChange) {
  // Subscribe the listener
  eventManager->subscribe(listener, EventType::RequestStateChange);

  // Set the context
  AppState state = AppState::EnterLocoAddress;
  int throttleIndex = 1;

  // Expectation
  EXPECT_CALL(
      *listener,
      onEvent(AllOf(Field(&Event::eventType, EventType::RequestStateChange),
                    Field(&Event::eventData, Field(&EventData::dataType, EventData::DataType::StateRequestData)),
                    Field(&Event::eventData, Field(&EventData::stateRequestValue,
                                                   AllOf(Field(&StateRequest::state, state),
                                                         Field(&StateRequest::contextIndex, throttleIndex)))))))
      .Times(1);

  // Publish the event
  EventData eventData(AppState::EnterLocoAddress, throttleIndex);
  eventManager->publish(EventType::RequestStateChange, eventData);

  // Verify and clear expectations
  Mock::VerifyAndClearExpectations(listener);
}

/**
 * @brief Test a ToggleLocoFunction event can be published and received by the listener
 */
TEST_F(EventManagerTests, TestToggleLocoFunction) {
  // Subscribe the listener
  eventManager->subscribe(listener, EventType::ToggleLocoFunction);

  // Set the context
  int function = 0;
  int throttleIndex = 1;
  UserInputInterface::UserInputAction action = UserInputInterface::UserInputAction::Pressed;

  // Expectation
  EXPECT_CALL(
      *listener,
      onEvent(AllOf(Field(&Event::eventType, EventType::ToggleLocoFunction),
                    Field(&Event::eventData, Field(&EventData::dataType, EventData::DataType::LocoFunctionData)),
                    Field(&Event::eventData,
                          Field(&EventData::locoFunctionValue, AllOf(Field(&LocoFunction::function, function),
                                                                    Field(&LocoFunction::throttleIndex, throttleIndex),
                                                                    Field(&LocoFunction::action, action)))))))
      .Times(1);

  // Publish the event
  EventData eventData(function, throttleIndex, UserInputInterface::UserInputAction::Pressed);
  eventManager->publish(EventType::ToggleLocoFunction, eventData);

  // Verify and clear expectations
  Mock::VerifyAndClearExpectations(listener);
}
