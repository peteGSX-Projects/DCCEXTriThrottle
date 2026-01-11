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

#ifndef MOCKDCCEXLOCO_H
#define MOCKDCCEXLOCO_H

#include "DCCEXLoco.h"
#include <gmock/gmock.h>

/**
 * @brief Google Mock implementation of the Loco class
 */
class MockLoco : public Loco {
public:
  MockLoco(int address, LocoSource source) : Loco(address, source) {}

  MOCK_METHOD(int, getAddress, (), (override));
  MOCK_METHOD(void, setName, (const char *name), (override));
  MOCK_METHOD(const char *, getName, (), (override));
  MOCK_METHOD(void, setSpeed, (int speed), (override));
  MOCK_METHOD(int, getSpeed, (), (override));
  MOCK_METHOD(void, setDirection, (Direction direction), (override));
  MOCK_METHOD(Direction, getDirection, (), (override));
  MOCK_METHOD(LocoSource, getSource, (), (override));
};

/**
 * @brief Google Mock implementation of the ConsistLoco class
 */
class MockConsistLoco : public ConsistLoco {
public:
  MockConsistLoco(Loco *loco, Facing facing) : ConsistLoco(loco, facing) {}

  MOCK_METHOD(Loco *, getLoco, (), (override));
  MOCK_METHOD(void, setFacing, (Facing facing), (override));
  MOCK_METHOD(Facing, getFacing, (), (override));
  MOCK_METHOD(ConsistLoco *, getNext, (), (override));
  MOCK_METHOD(void, setNext, (ConsistLoco * consistLoco), (override));
};

/**
 * @brief Google Mock implementation of the Consist class
 */
class MockConsist : public Consist {
public:
  MOCK_METHOD(void, setName, (const char *name), (override));
  MOCK_METHOD(const char *, getName, (), (override));
  MOCK_METHOD(int, getSpeed, (), (override));
  MOCK_METHOD(Direction, getDirection, (), (override));
  MOCK_METHOD(ConsistLoco *, getFirst, (), (override));
};

#endif // MOCKDCCEXLOCO_H
