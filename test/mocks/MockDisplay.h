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

#ifndef MOCKDISPLAY_H
#define MOCKDISPLAY_H

#include "DisplayInterface.h"
#include <gmock/gmock.h>

/**
 * @brief Mock for DisplayInterface
 */
class MockDisplay : public DisplayInterface {
public:
  /**
   * @brief Construct a new Mock Display object and ensure redraw methods always use the base class
   */
  MockDisplay() {
    ON_CALL(*this, setRedraw(testing::_)).WillByDefault(testing::Invoke([this](bool redraw) {
      this->DisplayInterface::setRedraw(redraw);
    }));

    ON_CALL(*this, needsRedraw()).WillByDefault(testing::Invoke([this]() {
      return this->DisplayInterface::needsRedraw();
    }));
  }

  MOCK_METHOD(void, begin, (), (override));

  MOCK_METHOD(void, clear, (), (override));

  MOCK_METHOD(void, displayThrottleScreen, (Throttle * *throttles), (override));

  MOCK_METHOD(void, updateThrottleScreen, (int throttleIndex, Throttle *throttles, bool force), (override));

  MOCK_METHOD(void, updateThrottleTrackPower, (TrackPower state), (override));

  MOCK_METHOD(void, displayMenuScreen, (Menu * menu), (override));

  MOCK_METHOD(void, displayConnectionErrorScreen, (), (override));

  MOCK_METHOD(void, displayProgressScreen, (const char *title, const char *message), (override));

  MOCK_METHOD(void, updateProgressScreen, (), (override));

  MOCK_METHOD(void, displayUserEntryScreen, (const char *title, const char *message), (override));

  MOCK_METHOD(void, displayUserEntryKey, (char key, int count), (override));

  MOCK_METHOD(void, setRedraw, (bool redraw), (override));

  MOCK_METHOD(bool, needsRedraw, (), (override));

private:
};

#endif // MOCKDISPLAY_H
