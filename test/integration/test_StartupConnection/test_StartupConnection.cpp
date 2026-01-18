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
 * @brief Test receiving DCCEXProtocol lists flags connection success
 */
TEST_F(IntegrationTestBase, TestStartupConnectionCompletes) {
  // update() should still be in Startup
  appOrchestrator->update();
  EXPECT_EQ(appOrchestrator->getCurrentAppState(), AppState::Startup);

  // Minimum of 4 updates required to process all lists and get connected
  for (int i = 0; i < 4; i++) {
    appOrchestrator->update();
  }

  // Should now flag connected and move to throttle
  EXPECT_EQ(appOrchestrator->getCurrentAppState(), AppState::Throttle);
}

/**
 * @brief Test connection success populates menu items
 */
TEST_F(IntegrationTestBase, TestStartupConnectionAppSetup) {

}
