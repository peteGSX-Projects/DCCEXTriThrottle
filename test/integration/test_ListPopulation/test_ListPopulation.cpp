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
 * @brief Test that the roster list populates the roster menu on CS connection
 */
TEST_F(IntegrationTestBase, TestRosterListPopulatesMenu) {
  // Complete the CS connection
  DCCEXTestHelpers::processCSConnection(appOrchestrator, csConnection);

  // We should be in Throttle state
  ASSERT_EQ(appOrchestrator->getCurrentAppState(), AppState::Throttle);

  // Navigate to the menu
  keypad->setInputEvent({'*', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();

  // Navigate to the roster menu
  keypad->setInputEvent({'6', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();

  // Ensure the list is populated
  Menu *roster = menuManager->getCurrentMenu();
  ASSERT_NE(roster, nullptr);

  // The first item should not be a nullptr
  ASSERT_NE(roster->getFirstItem(), nullptr);

  // First item should be Loco 1
  EXPECT_STREQ(roster->getFirstItem()->getName(), "Loco1");
}

/**
 * @brief Test that the turnout list populates the Turnout menu on CS connection
 */
TEST_F(IntegrationTestBase, TestTurnoutListPopulatesMenu) {
  // Complete the CS connection
  DCCEXTestHelpers::processCSConnection(appOrchestrator, csConnection);

  // We should be in Throttle state
  ASSERT_EQ(appOrchestrator->getCurrentAppState(), AppState::Throttle);

  // Navigate to the menu
  keypad->setInputEvent({'*', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();

  // Navigate to the turnout menu
  keypad->setInputEvent({'3', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();

  // Ensure the list is populated
  Menu *turnouts = menuManager->getCurrentMenu();
  ASSERT_NE(turnouts, nullptr);

  // The first item should not be a nullptr
  ASSERT_NE(turnouts->getFirstItem(), nullptr);

  // First item should be Loco 1
  EXPECT_STREQ(turnouts->getFirstItem()->getName(), "Turnout1");
}

/**
 * @brief Test that the route list populates the Route and Automation menus on CS connection
 */
TEST_F(IntegrationTestBase, TestRouteListPopulatesMenus) {
  // Complete the CS connection
  DCCEXTestHelpers::processCSConnection(appOrchestrator, csConnection);

  // We should be in Throttle state
  ASSERT_EQ(appOrchestrator->getCurrentAppState(), AppState::Throttle);

  // Navigate to the menu
  keypad->setInputEvent({'*', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();

  // Navigate to the turnout menu
  keypad->setInputEvent({'5', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();

  // Ensure the list is populated
  Menu *routeMenu = menuManager->getCurrentMenu();
  ASSERT_NE(routeMenu, nullptr);

  // The first item should not be a nullptr
  ASSERT_NE(routeMenu->getFirstItem(), nullptr);

  // First item should be Loco 1
  EXPECT_STREQ(routeMenu->getFirstItem()->getName(), "Route1");

  // // Back to the main menu, select throttle 1, and item 3 should be Automations
  keypad->setInputEvent({'*', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();
  keypad->setInputEvent({'0', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();

  // // Validate the menu is present
  Menu *throttleMenu = menuManager->getCurrentMenu();
  ASSERT_NE(throttleMenu->getItemByPageIndex(2), nullptr);
  EXPECT_STREQ(throttleMenu->getItemByPageIndex(2)->getName(), "Automations");

  // // Select menu and make sure first item is present
  keypad->setInputEvent({'2', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();
  Menu *automationMenu = menuManager->getCurrentMenu();
  ASSERT_NE(automationMenu->getFirstItem(), nullptr);
  EXPECT_STREQ(automationMenu->getFirstItem()->getName(), "Automation1");
}

/**
 * @brief Test that the turntable list populates the Turntable menu on CS connection
 */
TEST_F(IntegrationTestBase, TestTurntableListPopulatesMenu) {
  // Complete the CS connection
  DCCEXTestHelpers::processCSConnection(appOrchestrator, csConnection);

  // We should be in Throttle state
  ASSERT_EQ(appOrchestrator->getCurrentAppState(), AppState::Throttle);

  // Navigate to the menu
  keypad->setInputEvent({'*', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();

  // Navigate to the turntable menu
  keypad->setInputEvent({'4', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();

  // Ensure the list is populated
  Menu *turntables = menuManager->getCurrentMenu();
  ASSERT_NE(turntables, nullptr);

  // The first item should not be a nullptr
  ASSERT_NE(turntables->getFirstItem(), nullptr);

  // First item should be Turntable 1
  EXPECT_STREQ(turntables->getFirstItem()->getName(), "Turntable1");

  // Navigate to Turntable1
  keypad->setInputEvent({'0', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();

  // First item should not be a nullptr and should be Home
  ASSERT_NE(menuManager->getCurrentMenu()->getFirstItem(), nullptr);
  EXPECT_STREQ(menuManager->getCurrentMenu()->getFirstItem()->getName(), "Home");
}
