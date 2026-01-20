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
  // Create the mock roster
  csClient->createMockRoster();

  // update() needs to be called 5 times to complete connection
  for (int i = 0; i < 5; i++) {
    appOrchestrator->update();
  }

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
  EXPECT_STREQ(roster->getFirstItem()->getName(), "Loco 1");
}
