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

#include "ActionMenuItem.h"
#include "LocoMenuItem.h"
#include "Menu.h"
#include "SubMenuItem.h"
#include "ThrottleMenuItem.h"
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
  Loco *loco = new Loco(123, LocoSource::LocoSourceEntry);
  loco->setName("Loco 123");
  LocoMenuItem *item = new LocoMenuItem(loco);

  EXPECT_STREQ(item->getName(), "Loco 123");
  EXPECT_EQ(item->getIndex(), -1);
  EXPECT_EQ(item->getNext(), nullptr);
  EXPECT_EQ(item->getItemType(), MenuItemType::LocoType);

  // Check we can get the Loco
  EXPECT_EQ(item->getLoco(), loco);

  // Clean up
  delete item;
  delete loco;
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
  delete menu;
}

/**
 * @brief Test overriding a SubMenuItem's menu name
 */
TEST_F(MenuItemTests, SubMenuItemNameOverride) {
  // Create roster menu
  Menu *menu = new Menu("Roster");

  // Test default behaviour
  SubMenuItem *defaultName = new SubMenuItem(menu);
  EXPECT_STREQ(defaultName->getName(), "Roster");

  // Test override behaviour
  SubMenuItem *overrideName = new SubMenuItem(menu, "Select loco");
  EXPECT_STREQ(overrideName->getName(), "Select loco");

  delete overrideName;
  delete defaultName;
  delete menu;
}

/**
 * @brief Test the Throttle menu extending SubMenuItem
 */
TEST_F(MenuItemTests, ThrottleMenuItem) {
  // Create a menu and a ThrottleMenuItem and validate
  Menu *menu = new Menu("Test Throttle Menu");
  ThrottleMenuItem *item = new ThrottleMenuItem(menu, 1);

  EXPECT_STREQ(item->getName(), "Test Throttle Menu");
  EXPECT_EQ(item->getIndex(), -1);
  EXPECT_EQ(item->getNext(), nullptr);
  EXPECT_EQ(item->getItemType(), MenuItemType::ThrottleMenuType);
  EXPECT_EQ(item->getThrottleIndex(), 1);

  // Clean up
  delete item;
  delete menu;
}

/**
 * @brief Test the name override works for a throttle menu also
 */
TEST_F(MenuItemTests, ThrottleMenuItemNameOverride) {
  // Create a menu and a ThrottleMenuItem and validate
  Menu *menu = new Menu("Test Throttle Menu");

  // Expect default behaviour to use menu name
  ThrottleMenuItem *item = new ThrottleMenuItem(menu, 1);
  EXPECT_STREQ(item->getName(), "Test Throttle Menu");

  // Expect supplying a name to override
  ThrottleMenuItem *namedItem = new ThrottleMenuItem(menu, 1, "Override Name");
  EXPECT_STREQ(namedItem->getName(), "Override Name");

  // Clean up
  delete namedItem;
  delete item;
  delete menu;
}

/**
 * @brief Test an ActionMenuItem contains the correct attributes
 */
TEST_F(MenuItemTests, TestActionMenuItem) {
  // Dummy data
  int throttleIndex = 1;
  EventData data(throttleIndex);

  // Create the new item
  ActionMenuItem item("Enter address", EventType::LocoAddressEntered, data);

  // Verify attributes
  EXPECT_EQ(item.getEventType(), EventType::LocoAddressEntered);
  EXPECT_EQ(item.getEventData().intValue, throttleIndex);
}
