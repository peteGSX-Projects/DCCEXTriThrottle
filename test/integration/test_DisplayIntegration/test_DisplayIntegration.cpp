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
 * @brief Test interacting with a throttle updates the display
 */
TEST_F(IntegrationTestBase, TestThrottleDisplayUpdates) {
  // update() needs to be called 5 times to complete connection
  for (int i = 0; i < 5; i++) {
    appOrchestrator->update();
  }

  // We should be in Throttle state
  ASSERT_EQ(appOrchestrator->getCurrentAppState(), AppState::Throttle);
  
  // Set Loco
  throttles[0]->setLoco(new Loco(3, LocoSource::LocoSourceRoster));

  // Simulate increase speed
  encoder1->setInputAction(UserSelectionInterface::UserSelectionAction::Up);

  // Expect displayInterface->updateThrottleScreen to be called
  EXPECT_CALL(*display, updateThrottleScreen(0, _, _)).Times(1);

  appOrchestrator->update();

  // Verify flags were reset
  EXPECT_FALSE(throttles[0]->speedChanged());
}
