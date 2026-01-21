/*
 *  © 2025 Peter Cole
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

#include "Throttle.h"
#include "test/mocks/DCCEXProtocol.h"
#include "test/mocks/MockButton.h"
#include "test/mocks/MockRotaryEncoder.h"
#include "test/mocks/Stream.h"
#include <gtest/gtest.h>

using namespace testing;

class ThrottleTests : public Test {
protected:
  MockButton *button;
  MockRotaryEncoder *encoder;
  Throttle *throttle;
  Stream csConnection;
  DCCEXProtocol *csClient;

  // Optional setup method
  void SetUp() override {
    button = new MockButton;
    encoder = new MockRotaryEncoder;
    csClient = new DCCEXProtocol;
    csClient->connect(&csConnection);
    throttle = new Throttle(0, button, encoder, csClient, nullptr, 1, 2, 5);
  }

  // Optional teardown method
  void TearDown() override {
    delete csClient;
    delete throttle;
    delete encoder;
    delete button;
  }
};

/**
 * @brief Test the initial state of a Throttle instance is as expected
 */
TEST_F(ThrottleTests, TestInitialState) {
  EXPECT_EQ(throttle->getConsist(), nullptr);
  EXPECT_EQ(throttle->getLoco(), nullptr);
  EXPECT_EQ(throttle->getSpeed(), 0);
  EXPECT_EQ(throttle->speedChanged(), false);
  EXPECT_EQ(throttle->getDirection(), Direction::Forward);
  EXPECT_EQ(throttle->directionChanged(), false);
  EXPECT_EQ(throttle->locoChanged(), false);
}
