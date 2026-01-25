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
  // Create the mock roster
  csClient->createMockRoster();

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
  // Create the mock roster
  csClient->createMockRoster();

  // update() needs to be called 5 times to complete connection
  for (int i = 0; i < 5; i++) {
    appOrchestrator->update();
  }

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
  // update() needs to be called 5 times to complete connection
  for (int i = 0; i < 5; i++) {
    appOrchestrator->update();
  }

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
  // update() needs to be called 5 times to complete connection
  for (int i = 0; i < 5; i++) {
    appOrchestrator->update();
  }

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
