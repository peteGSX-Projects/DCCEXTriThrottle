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

#include "LocoMenuItem.h"
#include "Menu.h"
#include "SubMenuItem.h"
#include <gtest/gtest.h>

using namespace testing;

/// @brief Test harness for the various menu item tests
class MenuItemTests : public Test {
protected:
  void SetUp() override {}

  void TearDown() override {}
};

/// @brief Tests for the LocoMenuItem
TEST_F(MenuItemTests, LocoMenuItem) {
  // Test creating an item and validate it
  Loco *loco = new Loco();
  LocoMenuItem *item = new LocoMenuItem("Mock Loco", loco);

  EXPECT_STREQ(item->getName(), "Mock Loco");
  EXPECT_EQ(item->getIndex(), -1);
  EXPECT_EQ(item->getNext(), nullptr);
  EXPECT_EQ(item->getItemType(), MenuItemType::LocoType);

  // Clean up
  delete item;
}

/// @brief Tests for a menu item type containing another menu
TEST_F(MenuItemTests, SubMenuItem) {
  // Create a menu and a SubMenuItem and validate
  Menu *menu = new Menu("Test Menu");
  SubMenuItem *item = new SubMenuItem(menu);

  EXPECT_STREQ(item->getName(), "Test Menu");
  EXPECT_EQ(item->getIndex(), -1);
  EXPECT_EQ(item->getNext(), nullptr);
  EXPECT_EQ(item->getItemType(), MenuItemType::SubMenuType);

  // Clean up
  delete item;
}
