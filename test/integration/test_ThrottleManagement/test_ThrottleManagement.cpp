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
