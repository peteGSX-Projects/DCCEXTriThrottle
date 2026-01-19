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
  // Make sure buffer is clear to start
  csConnection.clear();
  
  // update() should send <JR>
  appOrchestrator->update();
  EXPECT_TRUE(csConnection.buffer.find("<JR>") != std::string::npos);
  csConnection.clear();

  // Next update() should send <JT>
  appOrchestrator->update();
  EXPECT_TRUE(csConnection.buffer.find("<JT>") != std::string::npos);
  csConnection.clear();

  // Next update() should send <JA>
  appOrchestrator->update();
  EXPECT_TRUE(csConnection.buffer.find("<JA>") != std::string::npos);
  csConnection.clear();

  // Fourth update() should send <JO>
  appOrchestrator->update();
  EXPECT_TRUE(csConnection.buffer.find("<JO>") != std::string::npos);
  csConnection.clear();

  // Final call should now see lists received and connect
  appOrchestrator->update();

  // We should now be in Throttle
  EXPECT_EQ(appOrchestrator->getCurrentAppState(), AppState::Throttle);
}

/**
 * @brief Test connection success populates menu items
 */
TEST_F(IntegrationTestBase, TestStartupConnectionAppSetup) {}
