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

#include "AppOrchestrator.h"
#include "CommandStationListener.h"
#include "EventManager.h"
#include "Throttle.h"
#include "Version.h"
#include "test/mocks/DCCEXTestHelpers.h"
#include "test/mocks/MockButton.h"
#include "test/mocks/MockConnectionManager.h"
#include "test/mocks/MockDisplay.h"
#include "test/mocks/MockKeypad.h"
#include "test/mocks/MockRotaryEncoder.h"
#include "test/mocks/Stream.h"
#include <DCCEXProtocol.h>
#include <gtest/gtest.h>

using namespace testing;

/// @brief Test harness for AppOrchestrator
class AppOrchestratorTests : public Test {
protected:
  AppOrchestrator *appOrchestrator;
  MockDisplay *mockDisplay;
  MockKeypad *mockKeypad;
  Logger *logger;
  MockButton *button1;
  MockButton *button2;
  MockButton *button3;
  MockRotaryEncoder *encoder1;
  MockRotaryEncoder *encoder2;
  MockRotaryEncoder *encoder3;
  MockConnectionManager *connectionManager;
  Throttle *throttles[NUM_THROTTLES];
  EventManager *eventManager;
  MenuManager *menuManager;
  Stream csConnection;
  Stream console;
  CommandStationListener *csListener;
  DCCEXProtocol *csClient;

  void SetUp() override {
    millis();
    mockDisplay = new MockDisplay;
    mockKeypad = new MockKeypad;
    logger = new Logger;
    logger->setOutput(&console);
    button1 = new MockButton;
    button2 = new MockButton;
    button3 = new MockButton;
    encoder1 = new MockRotaryEncoder;
    encoder2 = new MockRotaryEncoder;
    encoder3 = new MockRotaryEncoder;
    csClient = new DCCEXProtocol;
    csListener = new CommandStationListener(eventManager, logger);
    csClient->setDelegate(csListener);
    csClient->connect(&csConnection);
    throttles[0] = new Throttle(0, button1, encoder1, csClient, logger, 1, 2, 5);
    throttles[1] = new Throttle(1, button2, encoder2, csClient, logger, 1, 2, 5);
    throttles[2] = new Throttle(2, button3, encoder3, csClient, logger, 1, 2, 5);
    connectionManager = new MockConnectionManager;
    eventManager = new EventManager(nullptr);
    menuManager = new MenuManager(eventManager, logger);
    appOrchestrator = new AppOrchestrator(mockDisplay, mockKeypad, logger, NUM_THROTTLES, throttles, connectionManager,
                                          eventManager, menuManager, csClient);
  }

  void TearDown() override {
    delete appOrchestrator;
    delete menuManager;
    delete eventManager;
    delete connectionManager;

    for (int i = 0; i < 3; i++) {
      delete throttles[i];
    }

    console.clearInput();
    console.clearOutput();
    csConnection.clearInput();
    csConnection.clearOutput();
    csClient->clearAllLists();
    delete csClient;
    delete csListener;
    delete encoder3;
    delete encoder2;
    delete encoder1;
    delete button3;
    delete button2;
    delete button1;
    delete logger;
    delete mockKeypad;
    delete mockDisplay;
  }
};

/**
 * @brief Ensure AppOrchestrator starts in Startup state
 */
TEST_F(AppOrchestratorTests, ValidateStartupState) {
  EXPECT_EQ(appOrchestrator->getCurrentAppState(), AppState::Startup);
}

/**
 * @brief Ensure AppOrchestrator::begin() starts connection manager
 */
TEST_F(AppOrchestratorTests, TestBeginStartsConnectionManager) {
  // Set expectation
  EXPECT_CALL(*connectionManager, begin()).Times(1);
  appOrchestrator->begin();
}

/**
 * @brief Test AppState changes to Throttle automatically when connected
 */
TEST_F(AppOrchestratorTests, TestTransitionOnConnectionSuccess) {
  // update() should be called twice during this test
  EXPECT_CALL(*connectionManager, update()).Times(2);
  // getState() should return Connecting and then Connected
  EXPECT_CALL(*connectionManager, getState())
      .WillOnce(Return(ConnectionState::Connecting))
      .WillOnce(Return(ConnectionState::Connected));

  // We expect the Throttle screen to be displayed once
  EXPECT_CALL(*mockDisplay, displayThrottleScreen(_, _)).Times(1);

  // First update() should be Startup
  appOrchestrator->update();
  EXPECT_EQ(appOrchestrator->getCurrentAppState(), AppState::Startup);

  // Second update should be connected, and therefore move to Throttle
  appOrchestrator->update();
  EXPECT_EQ(appOrchestrator->getCurrentAppState(), AppState::Throttle);
}

/**
 * @brief Test AppState changes to ConnectionError automatically when connection fails
 */
TEST_F(AppOrchestratorTests, TestTransitionOnConnectionFail) {
  // update() should be called twice during this test
  EXPECT_CALL(*connectionManager, update()).Times(1);
  // getState() should return Failed
  EXPECT_CALL(*connectionManager, getState()).WillOnce(Return(ConnectionState::Failed));

  // We expect the Throttle screen to be displayed once
  EXPECT_CALL(*mockDisplay, displayConnectionErrorScreen()).Times(1);

  // First update() should be Startup
  appOrchestrator->update();
  EXPECT_EQ(appOrchestrator->getCurrentAppState(), AppState::ConnectionError);
}

/**
 * @brief Test user input causes connection retry
 */
TEST_F(AppOrchestratorTests, TestConnectionRetry) {
  // Force connection error
  EXPECT_CALL(*connectionManager, getState()).WillRepeatedly(Return(ConnectionState::Failed));
  appOrchestrator->update();
  ASSERT_EQ(appOrchestrator->getCurrentAppState(), AppState::ConnectionError);

  // When user presses any key (except * which is demo mode), should call connectionManager->begin()
  EXPECT_CALL(*connectionManager, begin()).Times(1);

  // Key press
  mockKeypad->setInputEvent({'1', UserInputInterface::UserInputAction::Pressed});

  // update()
  appOrchestrator->update();

  // Should be back in AppState::Startup now
  EXPECT_EQ(appOrchestrator->getCurrentAppState(), AppState::Startup);
}

/**
 * @brief Ensure AppOrchestrator event subscriptions work
 */
TEST_F(AppOrchestratorTests, TestEventSubscription) {
  // Call begin() to setup subscriptions
  appOrchestrator->begin();

  // Setup expectations
  EXPECT_CALL(*mockDisplay, updateProgressScreen()).Times(1);

  // Publish event
  eventManager->publish(EventType::ConnectionRetry, EventData());
}

/**
 * @brief Test successful transition from Throttle to Menu
 */
TEST_F(AppOrchestratorTests, TestThrottleToMenu) {
  // Connection success should have Throttle state
  EXPECT_CALL(*connectionManager, getState()).WillOnce(Return(ConnectionState::Connected));
  appOrchestrator->update();

  ASSERT_EQ(appOrchestrator->getCurrentAppState(), AppState::Throttle);

  // Now press '*' should move to Menu
  mockKeypad->setInputEvent({'*', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();
  EXPECT_EQ(appOrchestrator->getCurrentAppState(), AppState::Menu);
}

/**
 * @brief Test holding '0' will emergency stop all locos
 */
TEST_F(AppOrchestratorTests, TestEStopAllLocos) {
  // Make sure the buffer is clear first
  csConnection.clearOutput();

  // Long press of '0' in startup state should not send EStop
  mockKeypad->setInputEvent({'0', UserInputInterface::UserInputAction::Held});
  appOrchestrator->update();
  EXPECT_FALSE(csConnection.getOutput().find("<!>") != std::string::npos);
  csConnection.clearOutput();

  // Now set connection success to move to Throttle mode and it should work
  EXPECT_CALL(*connectionManager, getState()).WillOnce(Return(ConnectionState::Connected));
  appOrchestrator->update();

  ASSERT_EQ(appOrchestrator->getCurrentAppState(), AppState::Throttle);

  // Long press of '0' should now send EStop
  mockKeypad->setInputEvent({'0', UserInputInterface::UserInputAction::Held});
  appOrchestrator->update();
  EXPECT_TRUE(csConnection.getOutput().find("<!>") != std::string::npos);
  csConnection.clearOutput();
}

/**
 * @brief Test requesting an invalid state doesn't change states
 */
TEST_F(AppOrchestratorTests, TestInvalidateStateRequest) {
  // Set initial state to Menu
  appOrchestrator->setCurrentAppState(AppState::Menu);

  // Now set up the event with a non-existant AppState value
  int context = 1;
  EventData eventData((AppState)200, context);
  Event event(EventType::RequestStateChange, eventData);

  // Handle the event
  appOrchestrator->onEvent(event);

  // Validate the state hasn't changed
  EXPECT_EQ(appOrchestrator->getCurrentAppState(), AppState::Menu);
}

/**
 * @brief Test EnterLocoAddress state can be requested and displays correctly
 */
TEST_F(AppOrchestratorTests, TestRequestStateChangeToEnterAddress) {
  // Set up the event
  int throttleIndex = 1;
  EventData eventData(AppState::EnterLocoAddress, throttleIndex);
  Event event(EventType::RequestStateChange, eventData);

  // Set up expectation - throttle index 1 is throttle 2
  EXPECT_CALL(*mockDisplay, displayUserEntryScreen(StrEq("Throttle 2 Address"), StrEq("Enter DCC address:"))).Times(1);

  // Handle the event and call update to process the state
  appOrchestrator->onEvent(event);
  appOrchestrator->update();

  // Validate the outcome
  EXPECT_EQ(appOrchestrator->getCurrentAppState(), AppState::EnterLocoAddress);
}

/**
 * @brief Test manual entry of a loco address validates address and adds to throttle
 */
TEST_F(AppOrchestratorTests, TestManualLocoAddressEntry) {
  // Start in Menu state and set values
  appOrchestrator->setCurrentAppState(AppState::Menu);
  int throttleIndex = 1;
  int locoAddress = 1234;

  // Simulate the LocoAddressEntered event and handle it
  EventData eventData(locoAddress, throttleIndex);
  Event event(EventType::LocoAddressEntered, eventData);
  appOrchestrator->onEvent(event);

  // Validate the outcome
  EXPECT_EQ(appOrchestrator->getCurrentAppState(), AppState::Throttle);
  ASSERT_NE(throttles[throttleIndex]->getLoco(), nullptr);
  EXPECT_EQ(throttles[throttleIndex]->getLoco()->getAddress(), 1234);
  EXPECT_STREQ(throttles[throttleIndex]->getLoco()->getName(), "1234");
  EXPECT_EQ(throttles[throttleIndex]->getLoco()->getSource(), LocoSource::LocoSourceEntry);

  delete throttles[throttleIndex]->getLoco();
}

/**
 * @brief Test entering invalid loco addresses does not create a Loco
 */
TEST_F(AppOrchestratorTests, TestInvalidLocoAddressEntry) {
  // Start in Menu state and set values
  appOrchestrator->setCurrentAppState(AppState::Menu);
  int throttleIndex = 1;

  // Simulate the LocoAddressEntered event with an invalid address
  EventData eventData0(0, throttleIndex);
  Event event0(EventType::LocoAddressEntered, eventData0);
  appOrchestrator->onEvent(event0);

  // Validate the outcome
  EXPECT_EQ(appOrchestrator->getCurrentAppState(), AppState::EnterLocoAddress);
  EXPECT_EQ(throttles[throttleIndex]->getLoco(), nullptr);

  // Repeat with a too high address
  EventData eventData10240(10240, throttleIndex);
  Event event10240(EventType::LocoAddressEntered, eventData10240);
  appOrchestrator->onEvent(event10240);

  // Validate the outcome
  EXPECT_EQ(appOrchestrator->getCurrentAppState(), AppState::EnterLocoAddress);
  EXPECT_EQ(throttles[throttleIndex]->getLoco(), nullptr);
}

/**
 * @brief Test entering an invalid address displays the entry screen again with the error
 */
TEST_F(AppOrchestratorTests, TestInvalidAddressDisplay) {
  appOrchestrator->setCurrentAppState(AppState::EnterLocoAddress);

  // Set up the expectation
  EXPECT_CALL(*mockDisplay, displayUserEntryScreen(StrEq("Enter Address"), StrEq("Invalid address! Retry:"))).Times(1);

  // Handle the event
  EventData data(0, 1);
  Event event(EventType::LocoAddressEntered, data);
  appOrchestrator->onEvent(event);
}

/**
 * @brief Test user can only enter 1 to 5 digits
 */
TEST_F(AppOrchestratorTests, TestUserEntryDigitCount) {
  // Set state and context first
  appOrchestrator->setCurrentAppState(AppState::EnterLocoAddress);
  appOrchestrator->setActiveContextIndex(0);

  // Before any digits are entered, '#' should do nothing
  mockKeypad->setInputEvent({'#', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();
  EXPECT_EQ(appOrchestrator->getCurrentAppState(), AppState::EnterLocoAddress);

  // Enter 5 digits and expect 5 calls to displayUserEntryKey()
  EXPECT_CALL(*mockDisplay, displayUserEntryKey(_, _)).Times(5);
  for (int i = 0; i < 5; i++) {
    mockKeypad->setInputEvent({'1', UserInputInterface::UserInputAction::Pressed});
    appOrchestrator->update();
  }

  // Entering a 6th digit should not work and should not call displayUserEntryKey()
  EXPECT_CALL(*mockDisplay, displayUserEntryKey(_, _)).Times(0);
  mockKeypad->setInputEvent({'1', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();
}

/**
 * @brief Test entering digits correctly builds the address buffer
 */
TEST_F(AppOrchestratorTests, TestEnterAddressBufferBuilding) {
  // Set state and context first
  appOrchestrator->setCurrentAppState(AppState::EnterLocoAddress);
  appOrchestrator->setActiveContextIndex(0);

  // Expect the display be called with '1' and count 1
  EXPECT_CALL(*mockDisplay, displayUserEntryKey('1', 1)).Times(1);
  mockKeypad->setInputEvent({'1', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();

  // Expect the display be called with '5' and count 2
  EXPECT_CALL(*mockDisplay, displayUserEntryKey('5', 2)).Times(1);
  mockKeypad->setInputEvent({'5', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();
}

/**
 * @brief Test ReceivedLocoBroadcast updates a Loco object correctly
 */
TEST_F(AppOrchestratorTests, TestLocoBroadcastUpdatesThrottle) {
  // Set up loco 3 on throttle 0
  Loco *loco = new Loco(3, LocoSource::LocoSourceEntry);
  loco->setName("Loco 3");
  throttles[0]->setLoco(loco);

  // Create a broadcast event: loco 3, speed 25, reverse, and lights on
  LocoBroadcast broadcast = {3, 25, Direction::Reverse, 1};
  EventData eventData(broadcast);
  Event event(EventType::ReceivedLocoBroadcast, eventData);

  // Handle the event
  appOrchestrator->onEvent(event);

  // Throttle 0 should have incorrect details to start (0 and forward)
  EXPECT_EQ(throttles[0]->getSpeed(), 0);
  EXPECT_EQ(throttles[0]->getDirection(), Direction::Forward);

  // Actual Loco object should be correct
  EXPECT_EQ(throttles[0]->getLoco()->getSpeed(), 25);
  EXPECT_EQ(throttles[0]->getLoco()->getDirection(), Direction::Reverse);

  // The throttle should also have isSpeedPending() set as it has not updated
  EXPECT_TRUE(throttles[0]->isSpeedPending());

  // Advance time beyond the sync time (250ms) and update throttle
  advanceMillis(300);
  throttles[0]->update();

  // Throttle should now be correct and not pending
  EXPECT_EQ(throttles[0]->getSpeed(), 25);
  EXPECT_EQ(throttles[0]->getDirection(), Direction::Reverse);
  EXPECT_FALSE(throttles[0]->isSpeedPending());

  delete loco;
}

/**
 * @brief Test receiving a ReceivedTrackPower event updates the power status on the Throttle screen
 */
TEST_F(AppOrchestratorTests, TestReceivedTrackPowerUpdatesThrottleScreen) {
  // Set up
  TrackPower state = TrackPower::PowerOn;
  EventData eventData(state);
  Event event(EventType::ReceivedTrackPower, eventData);
  appOrchestrator->setCurrentAppState(AppState::Throttle);

  // Set the expectation
  EXPECT_CALL(*mockDisplay, updateThrottleTrackPower(state)).Times(1);

  // Handle the event
  appOrchestrator->onEvent(event);
}

/**
 * @brief Test receiving a ReceivedTrackPower event does not update when not in Throttle AppState
 */
TEST_F(AppOrchestratorTests, TestReceivedTrackPowerDoesNotUpdateOtherStates) {
  // Set up
  TrackPower state = TrackPower::PowerOn;
  EventData eventData(state);
  Event event(EventType::ReceivedTrackPower, eventData);
  appOrchestrator->setCurrentAppState(AppState::Menu);

  // Set the expectation
  EXPECT_CALL(*mockDisplay, updateThrottleTrackPower(state)).Times(0);

  // Handle the event
  appOrchestrator->onEvent(event);
}

/**
 * @brief Test toggling the power state from Unknown turns power on
 */
TEST_F(AppOrchestratorTests, TestTogglePowerUnknownSendsOn) {
  // Validate initial state is unknown to start
  ASSERT_EQ(appOrchestrator->getTrackPowerState(), TrackPower::PowerUnknown);

  // Set up the toggle event
  Event event(EventType::ToggleTrackPower, EventData());

  // Handle event
  appOrchestrator->onEvent(event);

  // Validate outcome
  EXPECT_EQ(csConnection.getOutput(), "<1>\r\n");

  // Should be back in Throttle state
  EXPECT_EQ(appOrchestrator->getCurrentAppState(), AppState::Throttle);
}

/**
 * @brief Test toggling the power state from on turns power off
 */
TEST_F(AppOrchestratorTests, TestTogglePowerOnSendsOff) {
  // Set power on from event and validate
  Event powerOnEvent(EventType::ReceivedTrackPower, EventData(TrackPower::PowerOn));
  appOrchestrator->onEvent(powerOnEvent);
  ASSERT_EQ(appOrchestrator->getTrackPowerState(), TrackPower::PowerOn);

  // Set up the toggle event
  Event toggleEvent(EventType::ToggleTrackPower, EventData());

  // Handle event
  appOrchestrator->onEvent(toggleEvent);

  // Validate outcome
  EXPECT_EQ(csConnection.getOutput(), "<0>\r\n");

  // Should be back in Throttle state
  EXPECT_EQ(appOrchestrator->getCurrentAppState(), AppState::Throttle);
}

/**
 * @brief Test toggling the power state from off turns power on
 */
TEST_F(AppOrchestratorTests, TestTogglePowerOffSendsOn) {
  // Set power on from event and validate
  Event powerOffEvent(EventType::ReceivedTrackPower, EventData(TrackPower::PowerOff));
  appOrchestrator->onEvent(powerOffEvent);
  ASSERT_EQ(appOrchestrator->getTrackPowerState(), TrackPower::PowerOff);

  // Set up the toggle event
  Event toggleEvent(EventType::ToggleTrackPower, EventData());

  // Handle event
  appOrchestrator->onEvent(toggleEvent);

  // Validate outcome
  EXPECT_EQ(csConnection.getOutput(), "<1>\r\n");

  // Should be back in Throttle state
  EXPECT_EQ(appOrchestrator->getCurrentAppState(), AppState::Throttle);
}

/**
 * @brief Test a ToggleTurnout event toggles the turnout state
 */
TEST_F(AppOrchestratorTests, TestToggleTurnout) {
  // Create a dummy turnout and add it to the client
  Turnout *turnout = new Turnout(1, false);
  csClient->turnouts = turnout;

  // Set up the toggle event
  Event toggleEvent(EventType::ToggleTurnout, EventData(turnout->getId()));

  // Handle the event
  appOrchestrator->onEvent(toggleEvent);

  // Simulate CS response and call update()
  csConnection << "<H 1 1>";
  csClient->check();

  // Turnout should now be thrown
  EXPECT_TRUE(turnout->getThrown());

  // Repeat handler and should not be thrown
  appOrchestrator->onEvent(toggleEvent);

  // Simulate CS response and call update()
  csConnection << "<H 1 0>";
  csClient->check();

  // Check again
  EXPECT_FALSE(turnout->getThrown());

  // Clean up
  delete turnout;
}

/**
 * @brief Test receiving a StartRoute event starts it
 */
TEST_F(AppOrchestratorTests, TestStartRoute) {
  // Set up event
  Event routeEvent(EventType::StartRoute, EventData(1));

  // Handle the event
  appOrchestrator->onEvent(routeEvent);

  // Check the outbound buffer for the correct string
  EXPECT_EQ(csConnection.getOutput(), "</ START 1>\r\n");
}

/**
 * @brief Test receiving a StartAutomation event starts it with the correct loco address
 */
TEST_F(AppOrchestratorTests, TestStartAutomation) {
  // Set up a dummy loco on throttle index 0
  Loco *loco = new Loco(1234, LocoSource::LocoSourceEntry);
  throttles[0]->setLoco(loco);

  // Setup a dummy automation as the CS validates the correct type
  Route *automation = new Route(4);
  automation->setType(RouteType::RouteTypeAutomation);
  csClient->routes = automation;

  // MenuManager must have the right throttle index
  menuManager->setActiveThrottleIndex(0);

  // Set up event
  Event automationEvent(EventType::StartAutomation, EventData(4));

  // Handle the event
  appOrchestrator->onEvent(automationEvent);

  // Check the outbound buffer for the correct string
  EXPECT_EQ(csConnection.getOutput(), "</ START 1234 4>\r\n");

  // AppOrchestrator should also return to Throttle state when starting an automation
  EXPECT_EQ(appOrchestrator->getCurrentAppState(), AppState::Throttle);

  // Clean up
  delete automation;
  delete loco;
}

/**
 * @brief Test receiving a RotateTurntable event triggers the turntable rotation
 */
TEST_F(AppOrchestratorTests, TestRotateTurntable) {
  // Set up the mock turntable list
  DCCEXTestHelpers::createMockTurntableList(csClient);

  // Validate the first turntable is at index 1
  Turntable *tt1 = csClient->turntables->getFirst();
  ASSERT_NE(tt1, nullptr);
  ASSERT_EQ(tt1->getIndex(), 1);

  // Set up the event to rotate turntable 1 to index 2
  Event tt1Event(EventType::RotateTurntable, EventData(1, 2));

  // Handle the event
  appOrchestrator->onEvent(tt1Event);

  // Check the outbound buffer for the rotate command - note it's a DCC turntable
  EXPECT_THAT(csConnection.getOutput(), StartsWith("<I 1 2>"));

  // Clear the buffer
  csConnection.clearOutput();

  // Now repeat for EX-Turntable type, rotate turntable 2 to position 1
  Turntable *tt2 = csClient->turntables->getById(2);

  // Validate currently at index 2
  ASSERT_NE(tt2, nullptr);
  ASSERT_EQ(tt2->getIndex(), 2);

  // Set up the event to rotate turntable 2 to index 1
  Event tt2Event(EventType::RotateTurntable, EventData(2, 1));

  // Handle the event
  appOrchestrator->onEvent(tt2Event);

  // Check the outbound buffer for the rotate command - note it's a DCC turntable
  EXPECT_THAT(csConnection.getOutput(), StartsWith("<I 2 1 0>"));
}

/**
 * @brief Test RequestStateChange to DisplaySysInfo event displays the System Info screen
 */
TEST_F(AppOrchestratorTests, TestDisplaySystemInfo) {
  // Setup the event
  EventData data(AppState::DisplaySysInfo, -1);
  Event event(EventType::RequestStateChange, data);

  // Set the expectation
  EXPECT_CALL(*mockDisplay, displaySysInfoScreen(_, _, _, _, _)).Times(1);

  // Handle the event
  appOrchestrator->onEvent(event);
  appOrchestrator->update();

  EXPECT_EQ(appOrchestrator->getCurrentAppState(), AppState::DisplaySysInfo);
}

/**
 * @brief Test ForgetLoco event clears the currently selected roster Loco from the Throttle index
 */
TEST_F(AppOrchestratorTests, TestForgetRosterLoco) {
  // Setup mock roster
  DCCEXTestHelpers::createMockRoster(csClient);

  // Associate the first loco with throttle index 2
  Loco *loco = csClient->roster->getFirst();
  throttles[2]->setLoco(loco);

  // Validate current state
  ASSERT_EQ(throttles[2]->getLoco(), loco);

  // Set up the event
  Event event(EventType::ForgetLoco, EventData(2));

  // Handle the event
  appOrchestrator->onEvent(event);

  // Validate throttle is cleared
  EXPECT_EQ(throttles[2]->getLoco(), nullptr);

  //
}

/**
 * @brief Test the ForgetLoco event deletes a local only Loco and clears from the Throttle index
 */
TEST_F(AppOrchestratorTests, TestForgetLocalLoco) {
  // Create local Loco and associate with throttle index 1
  Loco *localLoco = new Loco(3, LocoSource::LocoSourceEntry);
  throttles[1]->setLoco(localLoco);

  // Validate current state
  ASSERT_EQ(throttles[1]->getLoco(), localLoco);

  // Set up the event
  Event event(EventType::ForgetLoco, EventData(1));

  // Handle the event
  appOrchestrator->onEvent(event);

  // Validate throttle is cleared
  EXPECT_EQ(throttles[1]->getLoco(), nullptr);
}

/**
 * @brief Test the ForgetLoco event clears consist from the Throttle index
 */
TEST_F(AppOrchestratorTests, TestForgetConsist) {
  // Create the mock roster
  DCCEXTestHelpers::createMockRoster(csClient);

  // Create a local Loco
  Loco *localLoco = new Loco(5, LocoSource::LocoSourceEntry);

  // Create a consist with the first roster Loco and the local Loco
  Loco *rosterLoco = csClient->roster->getFirst();
  Consist *consist = new Consist();
  consist->addLoco(rosterLoco, Facing::FacingForward);
  consist->addLoco(localLoco, Facing::FacingReversed);

  // Validate consist
  ASSERT_NE(consist, nullptr);
  ASSERT_EQ(consist->getFirst()->getLoco(), rosterLoco);
  ASSERT_EQ(consist->getFirst()->getNext()->getLoco(), localLoco);

  // Associate the consist with Throttle index 0
  throttles[0]->setConsist(consist);

  // Validate
  ASSERT_EQ(throttles[0]->getConsist(), consist);

  // Set up the event
  Event event(EventType::ForgetLoco, EventData(0));

  // Handle the event
  appOrchestrator->onEvent(event);

  // Validate roster Loco still exists, local Loco is deleted, consist is deleted, and Throttle is clear
  EXPECT_EQ(throttles[0]->getConsist(), nullptr);
}

/**
 * @brief Test ForgetLoco for speed > 0 is ignored
 */
TEST_F(AppOrchestratorTests, TestForgetLocoAtSpeed) {
  // Setup mock roster
  DCCEXTestHelpers::createMockRoster(csClient);

  // Associate the first loco with throttle index 2 and set speed
  Loco *loco = csClient->roster->getFirst();
  throttles[2]->setLoco(loco);
  loco->setSpeed(10);

  // Validate current state
  ASSERT_EQ(throttles[2]->getLoco(), loco);
  EXPECT_EQ(throttles[2]->getLoco()->getSpeed(), 10);

  // Set up the event
  Event event(EventType::ForgetLoco, EventData(2));

  // Handle the event
  appOrchestrator->onEvent(event);

  // Validate is still associated with Throttle
  EXPECT_EQ(throttles[2]->getLoco(), loco);
}

/**
 * @brief Test ForgetLoco for an invalid throttle index is ignored
 */
TEST_F(AppOrchestratorTests, TestForgetLocoInvalidThrottle) {
  // Setup mock roster
  DCCEXTestHelpers::createMockRoster(csClient);

  // Associate the first loco with throttle index 2
  Loco *loco = csClient->roster->getFirst();
  throttles[2]->setLoco(loco);

  // Validate current state
  ASSERT_EQ(throttles[2]->getLoco(), loco);

  // Set up the event
  Event event(EventType::ForgetLoco, EventData(-1));

  // Handle the event
  appOrchestrator->onEvent(event);

  // Validate Loco is still associated with the throttle
  EXPECT_EQ(throttles[2]->getLoco(), loco);
}

/**
 * @brief Test changing loco/consist selection when speed > 0 is ignored
 */
TEST_F(AppOrchestratorTests, TestSelectLocoAtSpeed) {
  // Setup mock roster
  DCCEXTestHelpers::createMockRoster(csClient);

  // Associate the first loco with throttle index 0 and set speed
  Loco *loco = csClient->roster->getFirst();
  throttles[0]->setLoco(loco);
  loco->setSpeed(20);

  // Validate current state
  ASSERT_EQ(throttles[0]->getLoco(), loco);
  EXPECT_EQ(throttles[0]->getLoco()->getSpeed(), 20);

  // Set second roster loco as our attempt to select
  Loco *newLoco = loco->getNext();

  // Set up the event
  EventData data(newLoco, 0);
  Event event(EventType::LocoSelected, data);

  // Handle the event
  appOrchestrator->onEvent(event);

  // Validate first loco is still associated with Throttle
  EXPECT_EQ(throttles[0]->getLoco(), loco);
}

/**
 * @brief Test an unknown AppState logs an error
 */
TEST_F(AppOrchestratorTests, TestUnknownAppState) {
  // Set an unknown AppState
  appOrchestrator->setCurrentAppState(AppState::APP_STATE_COUNT);

  // Update, and the log should contain an error
  appOrchestrator->update();
  EXPECT_THAT(console.getOutput(), StartsWith("[ERR] AppOrchestrator::update(): Unknown AppState:"));
}

/**
 * @brief Test an unknown EventType logs an error
 */
TEST_F(AppOrchestratorTests, TestUnknownEventType) {
  // Set up event with an unknown type
  Event event(EventType::EVENT_TYPE_COUNT, EventData());

  // Handle the event and the log should contain an error
  appOrchestrator->onEvent(event);
  EXPECT_THAT(console.getOutput(), StartsWith("[ERR] AppOrchestrator::onEvent() unknown event:"));
}
