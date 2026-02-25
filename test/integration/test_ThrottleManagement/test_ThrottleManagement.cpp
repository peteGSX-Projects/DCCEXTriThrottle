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

#include "../IntegrationTestFixture.h"

/**
 * @brief Test selecting a Loco from a throttle menu associates correctly
 */
TEST_F(IntegrationTestBase, TestThrottleSelectsLoco) {
  // Complete the CS connection
  DCCEXTestHelpers::processCSConnection(appOrchestrator, csConnection);

  // We'll use Throttle 2 (index 1)
  int throttleIndex = 1;

  // Set up the event data to send
  EventData eventData(csClient->roster->getFirst(), throttleIndex);
  eventManager->publish(EventType::LocoSelected, eventData);

  // update() to process the event
  appOrchestrator->update();

  // Verify the first roster loco is now associated with Throttle 2
  ASSERT_NE(throttles[throttleIndex]->getLoco(), nullptr);
  EXPECT_EQ(throttles[throttleIndex]->getLoco()->getAddress(), 1);

  // Verify other throttles have no loco
  EXPECT_EQ(throttles[0]->getLoco(), nullptr);
  EXPECT_EQ(throttles[2]->getLoco(), nullptr);

  // AppOrchestrator should be in Throttle state
  EXPECT_EQ(appOrchestrator->getCurrentAppState(), AppState::Throttle);
}

/**
 * @brief Test selecting a loco resets the menu back to root
 */
TEST_F(IntegrationTestBase, TestMenuResetAfterSelectLoco) {
  // Complete the CS connection
  DCCEXTestHelpers::processCSConnection(appOrchestrator, csConnection);

  // We should be in Throttle state
  ASSERT_EQ(appOrchestrator->getCurrentAppState(), AppState::Throttle);

  // Navigate to the menu and first throttle instance
  keypad->setInputEvent({'*', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();
  keypad->setInputEvent({'0', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();

  // Ensure the first item is the first loco
  Menu *roster = menuManager->getCurrentMenu();
  ASSERT_NE(roster, nullptr);
  ASSERT_NE(roster->getFirstItem(), nullptr);
  ASSERT_STRNE(roster->getFirstItem()->getName(), "Loco 1");

  // Select first item which should go back to Throttle state
  keypad->setInputEvent({'0', UserInputInterface::UserInputAction::Pressed});
  // First update processes event
  appOrchestrator->update();
  // Second update changes state
  appOrchestrator->update();
  ASSERT_EQ(appOrchestrator->getCurrentAppState(), AppState::Throttle);

  // Navigate back to the menu and ensure it is the Main Menu
  keypad->setInputEvent({'*', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();

  EXPECT_STREQ(menuManager->getCurrentMenu()->getName(), "Main Menu");
}

/**
 * @brief Test selecting Enter Address from the menu changes state to EnterLocoAddress
 */
TEST_F(IntegrationTestBase, TestSelectEnterAddressChangesState) {
  // Complete the CS connection
  DCCEXTestHelpers::processCSConnection(appOrchestrator, csConnection);

  // We should be in Throttle state
  ASSERT_EQ(appOrchestrator->getCurrentAppState(), AppState::Throttle);

  // Navigate to the menu, first throttle instance, and item 1 should be Enter Address
  keypad->setInputEvent({'*', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();
  keypad->setInputEvent({'0', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();

  ASSERT_STREQ(menuManager->getCurrentMenu()->getItemByPageIndex(1)->getName(), "Enter Address");

  // Press 1 to select
  keypad->setInputEvent({'1', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();

  // Validate outcome
  EXPECT_EQ(appOrchestrator->getCurrentAppState(), AppState::EnterLocoAddress);
  EXPECT_EQ(appOrchestrator->getActiveContextIndex(), 0);
}

/**
 * @brief Test manually entering a loco address sets the loco for Throttle 1
 */
TEST_F(IntegrationTestBase, TestManualAddressEntry) {
  // Complete the CS connection
  DCCEXTestHelpers::processCSConnection(appOrchestrator, csConnection);

  // We should be in Throttle state
  ASSERT_EQ(appOrchestrator->getCurrentAppState(), AppState::Throttle);

  // Navigate to the menu, first throttle instance, and Enter Address
  keypad->setInputEvent({'*', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();
  keypad->setInputEvent({'0', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();
  keypad->setInputEvent({'1', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();

  // Enter loco address and press '#' to confirm
  keypad->setInputEvent({'3', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();
  keypad->setInputEvent({'#', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();

  // Validate outcome
  EXPECT_EQ(appOrchestrator->getCurrentAppState(), AppState::Throttle);
  ASSERT_NE(throttles[0]->getLoco(), nullptr);
  EXPECT_EQ(throttles[0]->getLoco()->getAddress(), 3);

  delete throttles[0]->getLoco();
}

/**
 * @brief Test turning lights on/off with '1', '2', '3' keys
 */
TEST_F(IntegrationTestBase, TestLocoLightFunctions) {
  // Connect so we are in throttle state and have a roster
  DCCEXTestHelpers::processCSConnection(appOrchestrator, csConnection);
  csConnection.clearOutput();

  Loco *loco1 = csClient->roster->getFirst();
  Loco *loco2 = loco1->getNext();
  Loco *loco3 = loco2->getNext();

  // All three must have function 0 off first
  ASSERT_FALSE(csClient->isFunctionOn(loco1, 0));
  ASSERT_FALSE(csClient->isFunctionOn(loco2, 0));
  ASSERT_FALSE(csClient->isFunctionOn(loco3, 0));

  // Associate locos with each throttle
  throttles[0]->setLoco(loco1);
  throttles[1]->setLoco(loco2);
  throttles[2]->setLoco(loco3);

  // Function 0 will be off by default, so sending '1', '2', and '3' should turn it on for the appropriate loco
  keypad->setInputEvent({'1', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();
  EXPECT_EQ(csConnection.getOutput(), "<F 1 0 1>");
  csConnection.clearOutput();
  keypad->setInputEvent({'2', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();
  EXPECT_EQ(csConnection.getOutput(), "<F 2 0 1>");
  csConnection.clearOutput();
  keypad->setInputEvent({'3', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();
  EXPECT_EQ(csConnection.getOutput(), "<F 3 0 1>");
  csConnection.clearOutput();

  // Simulate receiving the broadcasts which set functions on
  csConnection << "<l 1 0 128 1><l 2 0 128 1><l 3 0 128 1>";
  keypad->setInputEvent({'\0', UserInputInterface::UserInputAction::None});
  appOrchestrator->update();

  // Validate function 0 is now on for each
  ASSERT_TRUE(csClient->isFunctionOn(loco1, 0));
  ASSERT_TRUE(csClient->isFunctionOn(loco2, 0));
  ASSERT_TRUE(csClient->isFunctionOn(loco3, 0));

  // Repeat key presses which should send function off for '1', '2', '3' respectively
  keypad->setInputEvent({'1', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();
  EXPECT_EQ(csConnection.getOutput(), "<F 1 0 0>");
  csConnection.clearOutput();
  keypad->setInputEvent({'2', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();
  EXPECT_EQ(csConnection.getOutput(), "<F 2 0 0>");
  csConnection.clearOutput();
  keypad->setInputEvent({'3', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();
  EXPECT_EQ(csConnection.getOutput(), "<F 3 0 0>");
  csConnection.clearOutput();
}

/**
 * @brief Test turning lights on/off with '1', '2', '3' keys
 */
TEST_F(IntegrationTestBase, TestConsistLightFunctions) {
  // Connect so we are in throttle state and have a roster
  DCCEXTestHelpers::processCSConnection(appOrchestrator, csConnection);
  csConnection.clearOutput();

  Loco *loco1 = csClient->roster->getFirst();
  Loco *loco2 = loco1->getNext();
  Loco *loco3 = loco2->getNext();

  // All three must have function 0 off first
  ASSERT_FALSE(csClient->isFunctionOn(loco1, 0));
  ASSERT_FALSE(csClient->isFunctionOn(loco2, 0));
  ASSERT_FALSE(csClient->isFunctionOn(loco3, 0));

  // Create consist
  Consist *consist = new Consist();
  consist->addLoco(loco1, Facing::FacingForward);
  consist->addLoco(loco2, Facing::FacingReversed);
  consist->addLoco(loco3, Facing::FacingForward);

  // Associate consist with throttle 2
  throttles[2]->setConsist(consist);

  // Function 0 will be off by default, so sending '1', should turn it on for all locos
  keypad->setInputEvent({'3', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();
  EXPECT_EQ(csConnection.getOutput(), "<F 1 0 1><F 2 0 1><F 3 0 1>");
  csConnection.clearOutput();

  // Simulate receiving the broadcasts which set functions on
  csConnection << "<l 1 0 128 1><l 2 0 0 1><l 3 0 128 1>";
  keypad->setInputEvent({'\0', UserInputInterface::UserInputAction::None});
  appOrchestrator->update();

  // Validate function 0 is now on for each
  ASSERT_TRUE(csClient->isFunctionOn(loco1, 0));
  ASSERT_TRUE(csClient->isFunctionOn(loco2, 0));
  ASSERT_TRUE(csClient->isFunctionOn(loco3, 0));

  // Repeat key presses which should send function off for '1'
  keypad->setInputEvent({'3', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();
  EXPECT_EQ(csConnection.getOutput(), "<F 1 0 0><F 2 0 0><F 3 0 0>");

  // Clean up
  delete consist;
}