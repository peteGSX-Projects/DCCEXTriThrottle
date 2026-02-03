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
 * @brief Test selecting a Turntable index from the menu rotates the turntable
 */
TEST_F(IntegrationTestBase, TestSelectIndexRotatesTurntable) {
  // Complete the CS connection
  DCCEXTestHelpers::processCSConnection(appOrchestrator, csConnection);

  // Get the first turntable for testing
  Turntable *turntable = csClient->turntables->getFirst();
  ASSERT_NE(turntable, nullptr);

  // Should be at index 1 to start
  EXPECT_EQ(turntable->getIndex(), 1);

  // We should be in Throttle state
  ASSERT_EQ(appOrchestrator->getCurrentAppState(), AppState::Throttle);

  // Navigate to the menu, then turntable menu
  keypad->setInputEvent({'*', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();
  keypad->setInputEvent({'4', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();

  // Assert if nullptr
  ASSERT_NE(menuManager->getCurrentMenu(), nullptr);

  // Should be in Turntable menu, first item should be "Turntable1"
  EXPECT_STREQ(menuManager->getCurrentMenu()->getName(), "Turntables");
  EXPECT_STREQ(menuManager->getCurrentMenu()->getItemByPageIndex(0)->getName(), "Turntable1");

  // Press 0 to select Turntable1 menu and index 1 should be index 1
  keypad->setInputEvent({'0', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();
  ASSERT_NE(menuManager->getCurrentMenu()->getItemByPageIndex(1), nullptr);
  EXPECT_STREQ(menuManager->getCurrentMenu()->getItemByPageIndex(1)->getName(), "TT1 Index1");

  // Select to rotate turntable
  keypad->setInputEvent({'1', UserInputInterface::UserInputAction::Pressed});
  appOrchestrator->update();

  // Simulate CS response and update() - DCC turntable so moving always 0
  csConnection << "<I 1 1 0>";
  appOrchestrator->update();

  // Validate outcome, still in Menu state, turnout thrown
  EXPECT_EQ(appOrchestrator->getCurrentAppState(), AppState::Menu);
  EXPECT_EQ(turntable->getIndex(), 1);
}