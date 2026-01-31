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
 * @brief Test selecting a Turnout from the menu toggles the turnout
 */
TEST_F(IntegrationTestBase, TestSelectTurnoutTogglesTurnout) {
  // Create the mock turnout list
  csClient->createMockTurnoutList();
  // Get the first turnout for testing
  Turnout *turnout = csClient->turnouts->getFirst();
  ASSERT_NE(turnout, nullptr);

  // First turnout should be closed to start
  EXPECT_FALSE(turnout->getThrown());

  // update() needs to be called 5 times to complete connection
  for (int i = 0; i < 5; i++) {
    appOrchestrator->update();
  }

  // We should be in Throttle state
  ASSERT_EQ(appOrchestrator->getCurrentAppState(), AppState::Throttle);

  // Navigate to the menu, then turnout menu
  keypad->setInputEvent({'*', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();
  keypad->setInputEvent({'3', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();

  // Assert if nullptr
  ASSERT_NE(menuManager->getCurrentMenu(), nullptr);

  // Should be in throttle menu, first item should be "Turnout 1"
  EXPECT_STREQ(menuManager->getCurrentMenu()->getName(), "Turnouts");
  EXPECT_STREQ(menuManager->getCurrentMenu()->getItemByPageIndex(0)->getName(), "Turnout 1");

  // Press 0 to select and toggle
  keypad->setInputEvent({'0', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();

  // Validate outcome, still in Menu state, turnout thrown
  EXPECT_EQ(appOrchestrator->getCurrentAppState(), AppState::Menu);
  EXPECT_TRUE(turnout->getThrown());
}