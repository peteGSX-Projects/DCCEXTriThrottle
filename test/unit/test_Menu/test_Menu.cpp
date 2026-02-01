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

#include "Menu.h"
#include <gtest/gtest.h>

using namespace testing;

/// @brief Test harness for the various menu item tests
class MenuTests : public Test {
protected:
  void SetUp() override {}

  void TearDown() override {}
};

/// @brief Tests for the BaseMenuItem
TEST_F(MenuTests, NewMenu) {
  // Test creating a menu and validate it
  Menu *menu = new Menu("Test Menu");

  EXPECT_STREQ(menu->getName(), "Test Menu");

  // Ensure default 10 items per page is set
  EXPECT_EQ(menu->getItemsPerPage(), 10);

  // Clean up
  delete menu;
}

/// @brief Test creating a simple menu structure of items
TEST_F(MenuTests, SimpleMenuStructure) {
  Menu *mainMenu = new Menu("Main Menu");
  Loco *loco0 = new Loco(123, LocoSource::LocoSourceEntry);
  loco0->setName("Loco 123");
  Loco *loco1 = new Loco(234, LocoSource::LocoSourceEntry);
  loco1->setName("Loco 234");
  Loco *loco2 = new Loco(555, LocoSource::LocoSourceEntry);
  loco2->setName("555");
  mainMenu->addItem(new LocoMenuItem(loco0));
  mainMenu->addItem(new LocoMenuItem(loco1));
  mainMenu->addItem(new LocoMenuItem(loco2));

  // Validate each menu item has the correct auto generated index and name is
  // the Loco name
  int i = 0;
  for (LocoMenuItem *item = static_cast<LocoMenuItem *>(mainMenu->getFirstItem()); item;
       item = static_cast<LocoMenuItem *>(item->getNext())) {
    EXPECT_EQ(item->getIndex(), i);
    if (i == 0) {
      EXPECT_STREQ(item->getName(), loco0->getName());
    } else if (i == 1) {
      EXPECT_STREQ(item->getName(), loco1->getName());
    } else if (i == 2) {
      EXPECT_STREQ(item->getName(), "555");
    }
    i++;
  }

  // Clean up
  delete mainMenu;
  delete loco0;
  delete loco1;
  delete loco2;
}

/// @brief Test creating a menu with a submenu
TEST_F(MenuTests, MenuWithSubMenu) {
  // Create a main menu and a submenu with three items each
  Menu *mainMenu = new Menu("Main Menu");
  Menu *subMenu = new Menu("SubMenu");
  Loco *loco0 = new Loco(222, LocoSource::LocoSourceEntry);
  Loco *loco1 = new Loco(8, LocoSource::LocoSourceEntry);
  Loco *loco2 = new Loco(1, LocoSource::LocoSourceEntry);
  Loco *loco3 = new Loco(9999, LocoSource::LocoSourceEntry);
  Loco *loco4 = new Loco(12, LocoSource::LocoSourceEntry);
  mainMenu->addItem(new SubMenuItem(subMenu));
  mainMenu->addItem(new LocoMenuItem(loco0));
  mainMenu->addItem(new LocoMenuItem(loco1));
  subMenu->addItem(new LocoMenuItem(loco2));
  subMenu->addItem(new LocoMenuItem(loco3));
  subMenu->addItem(new LocoMenuItem(loco4));

  // Validate main menu items
  EXPECT_EQ(mainMenu->getFirstItem()->getItemType(), MenuItemType::SubMenuType);

  int i = 0;
  for (BaseMenuItem *item = mainMenu->getFirstItem(); item; item = item->getNext()) {
    EXPECT_EQ(item->getIndex(), i);
    i++;
  }

  // Clean up
  delete mainMenu;
  delete subMenu;
  delete loco0;
  delete loco1;
  delete loco2;
  delete loco3;
  delete loco4;
}

/**
 * @brief Test the pagination logic for a Menu object works
 */
TEST_F(MenuTests, TestPaginationLogic) {
  // Create the menu
  Menu *menu = new Menu("Pagination Test");
  Loco *dummyLoco = new Loco(3, LocoSource::LocoSourceEntry);

  // Add 22 items - 3 pages
  for (int i = 0; i < 22; i++) {
    menu->addItem(new LocoMenuItem(dummyLoco));
  }

  // Check initial attributes
  EXPECT_EQ(menu->getTotalPages(), 3);
  EXPECT_EQ(menu->getCurrentPage(), 0);

  // Navigate to the page 1
  menu->nextPage();
  EXPECT_EQ(menu->getCurrentPage(), 1);

  // Navigate to page 2
  menu->nextPage();
  EXPECT_EQ(menu->getCurrentPage(), 2);

  // Navigate back to page 0
  menu->nextPage();
  EXPECT_EQ(menu->getCurrentPage(), 0);

  // Cleanup
  delete menu;
  delete dummyLoco;
}

/**
 * @brief Test retrieving an item's index based on the current page and user selection
 */
TEST_F(MenuTests, TestGetItemByPageIndex) {
  // Create a menu with the default 10 items per page
  Menu *menu = new Menu("Selection Test");
  int itemsPerPage = menu->getItemsPerPage();

  // Create two separate Locos for testing
  Loco *locoPage0 = new Loco(100, LocoSource::LocoSourceEntry);
  locoPage0->setName("100");
  Loco *locoPage1 = new Loco(200, LocoSource::LocoSourceEntry);
  locoPage1->setName("200");

  // Fill page 0
  for (int i = 0; i < itemsPerPage; i++) {
    menu->addItem(new LocoMenuItem(locoPage0));
  }

  // Add a new one which goes to page 1
  menu->addItem(new LocoMenuItem(locoPage1));

  // Page 0 index 0 should return first Loco
  BaseMenuItem *item0 = menu->getItemByPageIndex(0);
  ASSERT_NE(item0, nullptr);
  EXPECT_STREQ(item0->getName(), "100");

  // Move to next page
  menu->nextPage();

  // Now index 0 should be second Loco
  BaseMenuItem *item1 = menu->getItemByPageIndex(0);
  ASSERT_NE(item1, nullptr);
  EXPECT_STREQ(item1->getName(), "200");

  // Test out of bounds
  EXPECT_EQ(menu->getItemByPageIndex(11), nullptr);

  delete menu;
  delete locoPage0;
  delete locoPage1;
}

/**
 * @brief Test clearItems() deletes all related items and resets Menu state
 */
TEST_F(MenuTests, TestClearItems) {
  // Create our test objects
  Menu *menu = new Menu("Clear Test");
  Loco *dummy = new Loco(1, LocoSource::LocoSourceEntry);

  // Add a bunch to the menu so we have two pages
  for (int i = 0; i < 15; i++) {
    menu->addItem(new LocoMenuItem(dummy));
  }

  // Navigate to the second page
  menu->nextPage();

  // Verify state is as we expect
  ASSERT_EQ(menu->getItemCount(), 15);
  EXPECT_EQ(menu->getCurrentPage(), 1);

  // Now clear and verify reset state
  menu->clearItems();

  EXPECT_EQ(menu->getItemCount(), 0);
  EXPECT_EQ(menu->getFirstItem(), nullptr);
  EXPECT_EQ(menu->getCurrentPage(), 0);
  EXPECT_EQ(menu->getTotalPages(), 1);

  // Verify adding a new item restarts correctly
  menu->addItem(new LocoMenuItem(dummy));
  EXPECT_EQ(menu->getFirstItem()->getIndex(), 0);

  delete menu;
  delete dummy;
}
