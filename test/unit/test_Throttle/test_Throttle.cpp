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
#include "test/mocks/MockButton.h"
#include "test/mocks/MockRotaryEncoder.h"
#include <gtest/gtest.h>

using namespace testing;

class ThrottleTests : public Test {
protected:
  MockButton button;
  MockRotaryEncoder encoder;
  Throttle throttle = Throttle(&button, &encoder);

  // Optional setup method
  void SetUp() override {}

  // Optional teardown method
  void TearDown() override {}
};

TEST_F(ThrottleTests, BasicTest) {}
