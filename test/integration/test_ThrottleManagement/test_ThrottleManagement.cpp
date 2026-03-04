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
TEST_F(IntegrationTestBase, TestLocoLightFunction) {
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
TEST_F(IntegrationTestBase, TestConsistLightFunction) {
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

  // Create consist with function replication enabled
  CSConsist *consist = csClient->createCSConsist(loco1->getAddress(), false, true);
  csClient->addCSConsistMember(consist, loco2->getAddress(), true);
  csClient->addCSConsistMember(consist, loco3->getAddress());
  // Clear output after creating CSConsist
  csConnection.clearOutput();

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

/**
 * @brief Test turning horn on/off with '4', '5', '6' keys
 */
TEST_F(IntegrationTestBase, TestLocoHornFunction) {
  // Connect so we are in throttle state and have a roster
  DCCEXTestHelpers::processCSConnection(appOrchestrator, csConnection);
  csConnection.clearOutput();

  Loco *loco1 = csClient->roster->getFirst();
  Loco *loco2 = loco1->getNext();
  Loco *loco3 = loco2->getNext();

  // All three must have function 1 off first and it should be momentary
  ASSERT_FALSE(csClient->isFunctionOn(loco1, 1));
  ASSERT_FALSE(csClient->isFunctionOn(loco2, 1));
  ASSERT_FALSE(csClient->isFunctionOn(loco3, 1));
  ASSERT_TRUE(loco1->isFunctionMomentary(1));
  ASSERT_TRUE(loco2->isFunctionMomentary(1));
  ASSERT_TRUE(loco3->isFunctionMomentary(1));

  // Associate locos with each throttle
  throttles[0]->setLoco(loco1);
  throttles[1]->setLoco(loco2);
  throttles[2]->setLoco(loco3);

  // Function 1 will be off by default, so holding '4', '5', and '6' should turn it on for the appropriate loco
  keypad->setInputEvent({'4', UserInputInterface::UserInputAction::Held});
  appOrchestrator->update();
  EXPECT_EQ(csConnection.getOutput(), "<F 1 1 1>");
  csConnection.clearOutput();
  keypad->setInputEvent({'5', UserInputInterface::UserInputAction::Held});
  appOrchestrator->update();
  EXPECT_EQ(csConnection.getOutput(), "<F 2 1 1>");
  csConnection.clearOutput();
  keypad->setInputEvent({'6', UserInputInterface::UserInputAction::Held});
  appOrchestrator->update();
  EXPECT_EQ(csConnection.getOutput(), "<F 3 1 1>");
  csConnection.clearOutput();

  // Simulate receiving the broadcasts which set functions on
  csConnection << "<l 1 0 128 2><l 2 0 128 2><l 3 0 128 2>";
  appOrchestrator->update();

  // Validate function 1 is now on for each
  ASSERT_TRUE(csClient->isFunctionOn(loco1, 1));
  ASSERT_TRUE(csClient->isFunctionOn(loco2, 1));
  ASSERT_TRUE(csClient->isFunctionOn(loco3, 1));

  // Repeat key presses which should send function off for '4', '5', '6' respectively
  keypad->setInputEvent({'4', UserInputInterface::UserInputAction::Released});
  appOrchestrator->update();
  EXPECT_EQ(csConnection.getOutput(), "<F 1 1 0>");
  csConnection.clearOutput();
  keypad->setInputEvent({'5', UserInputInterface::UserInputAction::Released});
  appOrchestrator->update();
  EXPECT_EQ(csConnection.getOutput(), "<F 2 1 0>");
  csConnection.clearOutput();
  keypad->setInputEvent({'6', UserInputInterface::UserInputAction::Released});
  appOrchestrator->update();
  EXPECT_EQ(csConnection.getOutput(), "<F 3 1 0>");
  csConnection.clearOutput();
}

/**
 * @brief Test turning horn on/off with '4', '5', '6' keys
 */
TEST_F(IntegrationTestBase, TestConsistHornFunction) {
  // Connect so we are in throttle state and have a roster
  DCCEXTestHelpers::processCSConnection(appOrchestrator, csConnection);
  csConnection.clearOutput();

  Loco *loco1 = csClient->roster->getFirst();
  Loco *loco2 = loco1->getNext();
  Loco *loco3 = loco2->getNext();

  // All three must have function 1 off first
  ASSERT_FALSE(csClient->isFunctionOn(loco1, 1));
  ASSERT_FALSE(csClient->isFunctionOn(loco2, 1));
  ASSERT_FALSE(csClient->isFunctionOn(loco3, 1));

  // Create consist with function replication enabled
  CSConsist *consist = csClient->createCSConsist(loco1->getAddress(), false, true);
  csClient->addCSConsistMember(consist, loco2->getAddress(), true);
  csClient->addCSConsistMember(consist, loco3->getAddress());
  // Clear output after creating CSConsist
  csConnection.clearOutput();

  // Associate consist with throttle 2
  throttles[2]->setConsist(consist);

  // Function 1 will be off by default, so sending '1', should turn it on for all locos
  keypad->setInputEvent({'6', UserInputInterface::UserInputAction::Held});
  appOrchestrator->update();
  EXPECT_EQ(csConnection.getOutput(), "<F 1 1 1><F 2 1 1><F 3 1 1>");
  csConnection.clearOutput();

  // Simulate receiving the broadcasts which set functions on
  csConnection << "<l 1 0 128 2><l 2 0 0 2><l 3 0 128 2>";
  appOrchestrator->update();

  // Validate function 1 is now on for each
  ASSERT_TRUE(csClient->isFunctionOn(loco1, 1));
  ASSERT_TRUE(csClient->isFunctionOn(loco2, 1));
  ASSERT_TRUE(csClient->isFunctionOn(loco3, 1));

  // Repeat key presses which should send function off for '6'
  keypad->setInputEvent({'6', UserInputInterface::UserInputAction::Released});
  appOrchestrator->update();
  EXPECT_EQ(csConnection.getOutput(), "<F 1 1 0><F 2 1 0><F 3 1 0>");

  // Clean up
  delete consist;
}

/**
 * @brief Test navigating to the function menu from Throttle and activating functions
 */
TEST_F(IntegrationTestBase, TestLocoFunctionMenu) {
  // Connect so we are in throttle state and have a roster
  DCCEXTestHelpers::processCSConnection(appOrchestrator, csConnection);
  csConnection.clearOutput();

  Loco *loco1 = csClient->roster->getFirst();
  Loco *loco2 = loco1->getNext();
  Loco *loco3 = loco2->getNext();

  // Associate locos with each throttle
  throttles[0]->setLoco(loco1);
  throttles[1]->setLoco(loco2);
  throttles[2]->setLoco(loco3);

  // '7', '8', and '9' should show function menu for each, with 0 turning lights on/off and 1 horn
  keypad->setInputEvent({'7', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();
  ASSERT_EQ(appOrchestrator->getCurrentAppState(), AppState::Menu);
  Menu *functionMenu = menuManager->getCurrentMenu();
  EXPECT_STREQ(functionMenu->getName(), "Loco1");
  EXPECT_STREQ(functionMenu->getItemByPageIndex(0)->getName(), "Func0");
  EXPECT_STREQ(functionMenu->getItemByPageIndex(1)->getName(), "*Func1");
  keypad->setInputEvent({'0', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();
  EXPECT_EQ(csConnection.getOutput(), "<F 1 0 1>");
  csConnection.clearOutput();
  keypad->setInputEvent({'1', UserInputInterface::UserInputAction::Held});
  appOrchestrator->update();
  EXPECT_EQ(csConnection.getOutput(), "<F 1 1 1>");
  csConnection.clearOutput();

  // Return to throttle with '*' and repeat
  keypad->setInputEvent({'*', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();
  ASSERT_EQ(appOrchestrator->getCurrentAppState(), AppState::Throttle);

  keypad->setInputEvent({'8', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();
  ASSERT_EQ(appOrchestrator->getCurrentAppState(), AppState::Menu);
  functionMenu = menuManager->getCurrentMenu();
  EXPECT_STREQ(functionMenu->getName(), "Loco2");
  EXPECT_STREQ(functionMenu->getItemByPageIndex(0)->getName(), "Func0");
  EXPECT_STREQ(functionMenu->getItemByPageIndex(1)->getName(), "*Func1");
  keypad->setInputEvent({'0', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();
  EXPECT_EQ(csConnection.getOutput(), "<F 2 0 1>");
  csConnection.clearOutput();
  keypad->setInputEvent({'1', UserInputInterface::UserInputAction::Held});
  appOrchestrator->update();
  EXPECT_EQ(csConnection.getOutput(), "<F 2 1 1>");
  csConnection.clearOutput();

  // Return to throttle with '*' and repeat
  keypad->setInputEvent({'*', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();
  ASSERT_EQ(appOrchestrator->getCurrentAppState(), AppState::Throttle);

  keypad->setInputEvent({'9', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();
  ASSERT_EQ(appOrchestrator->getCurrentAppState(), AppState::Menu);
  functionMenu = menuManager->getCurrentMenu();
  EXPECT_STREQ(functionMenu->getName(), "Loco3");
  EXPECT_STREQ(functionMenu->getItemByPageIndex(0)->getName(), "Func0");
  EXPECT_STREQ(functionMenu->getItemByPageIndex(1)->getName(), "*Func1");
  keypad->setInputEvent({'0', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();
  EXPECT_EQ(csConnection.getOutput(), "<F 3 0 1>");
  csConnection.clearOutput();
  keypad->setInputEvent({'1', UserInputInterface::UserInputAction::Held});
  appOrchestrator->update();
  EXPECT_EQ(csConnection.getOutput(), "<F 3 1 1>");
  csConnection.clearOutput();

  // Return to throttle with '*' and repeat
  keypad->setInputEvent({'*', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();
  ASSERT_EQ(appOrchestrator->getCurrentAppState(), AppState::Throttle);
}

/**
 * @brief Test toggling F28 works
 */
TEST_F(IntegrationTestBase, TestHighestLocoFunction) {
  // Connect so we are in throttle state and have a roster
  DCCEXTestHelpers::processCSConnection(appOrchestrator, csConnection);
  csConnection.clearOutput();

  Loco *loco1 = csClient->roster->getFirst();

  // Associate loco with a throttle
  throttles[0]->setLoco(loco1);

  // '7' should show function menu, navigate to page 3
  keypad->setInputEvent({'7', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();
  Menu *functionMenu = menuManager->getCurrentMenu();
  ASSERT_EQ(appOrchestrator->getCurrentAppState(), AppState::Menu);
  keypad->setInputEvent({'#', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();
  appOrchestrator->update();
  ASSERT_STREQ(functionMenu->getItemByPageIndex(8)->getName(), "F28");
  keypad->setInputEvent({'8', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();
  EXPECT_EQ(csConnection.getOutput(), "<F 1 28 1>");
  csConnection.clearOutput();
  keypad->setInputEvent({'\0', UserInputInterface::UserInputAction::None});

  // Simulate CS updating F28 to on
  csConnection << "<l 1 0 128 268435456>";
  appOrchestrator->update();
  ASSERT_TRUE(loco1->isFunctionOn(28));

  // Press 8 again to turn off
  keypad->setInputEvent({'8', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();
  EXPECT_EQ(csConnection.getOutput(), "<F 1 28 0>");
  csConnection.clearOutput();
}
