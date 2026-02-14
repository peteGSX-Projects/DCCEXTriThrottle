/*
 *  © 2026 Peter Cole
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

#include "Arduino.h"
#include "Throttle.h"
#include "test/mocks/MockButton.h"
#include "test/mocks/MockRotaryEncoder.h"
#include "test/mocks/Stream.h"
#include <DCCEXProtocol.h>
#include <gtest/gtest.h>

using namespace testing;

class ThrottleTests : public Test {
protected:
  MockButton *button;
  MockRotaryEncoder *encoder;
  Throttle *throttle;
  Stream csConnection;
  DCCEXProtocol *csClient;
  Loco *loco3;
  Loco *loco100;
  Consist *consist;

  // Optional setup method
  void SetUp() override {
    millis();
    button = new MockButton;
    encoder = new MockRotaryEncoder;
    csClient = new DCCEXProtocol;
    csClient->connect(&csConnection);
    throttle = new Throttle(0, button, encoder, csClient, nullptr, 1, 2, 5);
    loco3 = new Loco(3, LocoSource::LocoSourceEntry);
    loco100 = new Loco(100, LocoSource::LocoSourceEntry);
    consist = new Consist;
    consist->addLoco(loco3, Facing::FacingForward);
    consist->addLoco(loco100, Facing::FacingReversed);
  }

  // Optional teardown method
  void TearDown() override {
    resetMillis();
    delete consist;
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

/**
 * @brief Test the speed changes by the correct increments
 */
TEST_F(ThrottleTests, TestSpeedChanges) {
  throttle->setLoco(loco3);

  // Simulate up one normal speed
  encoder->setInputAction(UserSelectionInterface::UserSelectionAction::Up);

  // Validate change
  throttle->update();
  EXPECT_EQ(throttle->getSpeed(), 1);
  EXPECT_TRUE(throttle->speedChanged());

  // Up faster
  encoder->setInputAction(UserSelectionInterface::UserSelectionAction::UpFaster);
  throttle->update();
  EXPECT_EQ(throttle->getSpeed(), 3);
  EXPECT_TRUE(throttle->speedChanged());

  // Up fastest
  // Up faster
  encoder->setInputAction(UserSelectionInterface::UserSelectionAction::UpFastest);
  throttle->update();
  EXPECT_EQ(throttle->getSpeed(), 8);
  EXPECT_TRUE(throttle->speedChanged());
}

/**
 * @brief Test DCC speed limits 0 - 126 are enforced
 */
TEST_F(ThrottleTests, TestDCCSpeedLimits) {
  throttle->setLoco(loco100);

  // Attempt to increase speed beyond 126
  for (int i = 0; i < 130; i++) {
    encoder->setInputAction(UserSelectionInterface::UserSelectionAction::Up);
    throttle->update();
  }

  // Validate top speed is 126
  EXPECT_EQ(throttle->getSpeed(), 126);

  // Attempt to decrease below 0
  for (int i = 0; i < 130; i++) {
    encoder->setInputAction(UserSelectionInterface::UserSelectionAction::Down);
    throttle->update();
  }

  // Validate speed is now 0
  EXPECT_EQ(throttle->getSpeed(), 0);
}

/**
 * @brief When the speed is 0, single button press should change direction
 */
TEST_F(ThrottleTests, TestDirectionChange) {
  // Test startup is forward
  throttle->setLoco(loco3);
  EXPECT_EQ(throttle->getDirection(), Direction::Forward);

  // Button click should reverse
  button->setInputAction(UserConfirmationInterface::UserConfirmationAction::SingleClick);
  throttle->update();
  EXPECT_EQ(throttle->getDirection(), Direction::Reverse);
  EXPECT_TRUE(throttle->directionChanged());
  throttle->resetDirectionChanged();

  // Reset button action before speed change
  button->setInputAction(UserConfirmationInterface::UserConfirmationAction::None);

  // Now increase speed and single click should not change direction
  encoder->setInputAction(UserSelectionInterface::UserSelectionAction::UpFastest);
  throttle->update();
  EXPECT_EQ(throttle->getSpeed(), 5);
  button->setInputAction(UserConfirmationInterface::UserConfirmationAction::SingleClick);
  throttle->update();
  EXPECT_EQ(throttle->getDirection(), Direction::Reverse);
  EXPECT_FALSE(throttle->directionChanged());
}

/**
 * @brief Test an encoder single click sets speed to 0
 */
TEST_F(ThrottleTests, TestNormalStop) {
  throttle->setLoco(loco3);

  // Set a speed
  encoder->setInputAction(UserSelectionInterface::UserSelectionAction::UpFastest);
  throttle->update();
  EXPECT_EQ(throttle->getSpeed(), 5);

  // Clean encoder input and do single click
  encoder->setInputAction(UserSelectionInterface::UserSelectionAction::None);
  button->setInputAction(UserConfirmationInterface::UserConfirmationAction::SingleClick);
  throttle->update();
  EXPECT_EQ(throttle->getSpeed(), 0);
  EXPECT_EQ(throttle->getDirection(), Direction::Forward);
  EXPECT_TRUE(throttle->speedChanged());
}

/**
 * @brief Test an encoder long click performs EStop
 */
TEST_F(ThrottleTests, TestEStop) {
  throttle->setLoco(loco100);

  // Set a speed
  encoder->setInputAction(UserSelectionInterface::UserSelectionAction::UpFastest);
  throttle->update();
  EXPECT_EQ(throttle->getSpeed(), 5);

  // Clean encoder input and do long click
  encoder->setInputAction(UserSelectionInterface::UserSelectionAction::None);
  button->setInputAction(UserConfirmationInterface::UserConfirmationAction::LongClick);
  throttle->update();
  EXPECT_EQ(throttle->getSpeed(), -1);
  EXPECT_EQ(throttle->getDirection(), Direction::Forward);
  EXPECT_TRUE(throttle->speedChanged());
}

/**
 * @brief Test the speed pending flag is set/reset correctly by user input vs. loco speed
 */
TEST_F(ThrottleTests, TestSpeedPendingLogic) {
  // Set the initial loco speed to 10
  loco3->setSpeed(10);
  throttle->setLoco(loco3);
  throttle->update();

  // User increases speed
  encoder->setInputAction(UserSelectionInterface::UserSelectionAction::Up);
  throttle->update();

  EXPECT_EQ(throttle->getSpeed(), 11);
  EXPECT_EQ(loco3->getSpeed(), 10);
  EXPECT_TRUE(throttle->isSpeedPending());

  // CS now confirms speed
  loco3->setSpeed(11);
  EXPECT_FALSE(throttle->isSpeedPending());
}

/**
 * @brief Test that external changes to the Loco speed update the Throttle speed
 */
TEST_F(ThrottleTests, TestExternalSpeedUpdate) {
  throttle->setLoco(loco3);
  loco3->setSpeed(20);

  // Simulate user input
  encoder->setInputAction(UserSelectionInterface::UserSelectionAction::Up);
  throttle->update();
  encoder->setInputAction(UserSelectionInterface::UserSelectionAction::None);

  // Slight timer increment shouldn't sync speeds yet
  advanceMillis(100);
  throttle->update();
  EXPECT_NE(throttle->getSpeed(), 20);

  // Advance beyond 250ms threshold for sync
  advanceMillis(200);
  throttle->update();
  EXPECT_EQ(throttle->getSpeed(), 20);
  EXPECT_FALSE(throttle->isSpeedPending());
}

/**
 * @brief Test forget loco deletes local local and unsets from throttle
 */
TEST_F(ThrottleTests, TestForgetLoco) {
  // Assign mock loco 10
  Loco *loco10 = new Loco(10, LocoSource::LocoSourceEntry);
  throttle->setLoco(loco10);
  ASSERT_EQ(throttle->getLoco(), loco10);

  // Forget it
  throttle->forgetLoco();

  // Validate
  ASSERT_EQ(throttle->getLoco(), nullptr);
}

/**
 * @brief Test forget loco deletes consist and unsets from throttle
 */
TEST_F(ThrottleTests, TestForgetConsist) {
  // Create and assign mock consist
  Loco *loco10 = new Loco(10, LocoSource::LocoSourceEntry);
  Loco *loco11 = new Loco(11, LocoSource::LocoSourceEntry);
  Consist *mockConsist = new Consist;
  mockConsist->addLoco(loco10, Facing::FacingForward);
  mockConsist->addLoco(loco11, Facing::FacingReversed);
  throttle->setConsist(mockConsist);
  ASSERT_EQ(throttle->getConsist(), mockConsist);

  // Forget it
  throttle->forgetLoco();

  // Validate
  ASSERT_EQ(throttle->getConsist(), nullptr);
}

/**
 * @brief Test forgetting a loco at speed > 0 is ignored
 */
TEST_F(ThrottleTests, TestForgetLocoAtSpeed) {
  // Assign loco 3 and set speed
  loco3->setSpeed(10);
  throttle->setLoco(loco3);
  ASSERT_EQ(throttle->getLoco(), loco3);
  EXPECT_EQ(throttle->getSpeed(), 10);

  // Forget it
  throttle->forgetLoco();

  // Validate
  ASSERT_EQ(throttle->getLoco(), loco3);
}
