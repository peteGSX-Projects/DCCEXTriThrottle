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

#include "AppConfiguration.h"
#include "test/mocks/Stream.h"
#include <gtest/gtest.h>

using namespace testing;

/// @brief Test harness for AppConfiguration
class AppConfigurationTests : public Test {
protected:
  AppConfiguration *appConfig;
  Stream *consoleStream;
  Stream *commandStationStream;

  void SetUp() override {
    consoleStream = new Stream;
    commandStationStream = new Stream;
    appConfig = new AppConfiguration(consoleStream, commandStationStream);
  }

  void TearDown() override {
    delete consoleStream;
    delete commandStationStream;
    delete appConfig;
  }
};

/**
 * @brief Test initial object states are as expected
 */
TEST_F(AppConfigurationTests, TestInitialStates) {
  EXPECT_EQ(appConfig->getUserInputInterface(), nullptr);
  EXPECT_EQ(appConfig->getDisplayInterface(), nullptr);
  
  Throttle **throttles = appConfig->getThrottles();
  EXPECT_NE(throttles, nullptr);
  for (int i = 0; i < NUM_THROTTLES; i++) {
    EXPECT_EQ(throttles[i], nullptr);
  }

  EXPECT_NE(appConfig->getEventManager(), nullptr);
  EXPECT_NE(appConfig->getLogger(), nullptr);
}

/**
 * @brief Test the same instance is always retrieved
 */
TEST_F(AppConfigurationTests, TestSingleInstances) {
  EventManager *em1 = appConfig->getEventManager();
  EventManager *em2 = appConfig->getEventManager();
  EXPECT_EQ(em1, em2);

  Logger *l1 = appConfig->getLogger();
  Logger *l2 = appConfig->getLogger();
  EXPECT_EQ(l1, l2);
}
