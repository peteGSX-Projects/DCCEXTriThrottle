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
  EXPECT_EQ(menu->getParent(), nullptr);

  // Clean up
  delete menu;
}

/// @brief Test basic nested menus
TEST_F(MenuTests, NestedMenus) {
  // Create three menus, nest them, and validate
  Menu *mainMenu = new Menu("Main Menu");
  Menu *nestedMenu1 = new Menu("First Nested Menu");
  Menu *nestedMenu2 = new Menu("Second Nested Menu");

  nestedMenu1->setParent(mainMenu);
  nestedMenu2->setParent(nestedMenu1);

  EXPECT_STREQ(mainMenu->getName(), "Main Menu");
  EXPECT_EQ(mainMenu->getParent(), nullptr);
  EXPECT_EQ(nestedMenu1->getParent(), mainMenu);
  EXPECT_EQ(nestedMenu2->getParent(), nestedMenu1);

  // Clean up
  delete mainMenu;
  delete nestedMenu1;
  delete nestedMenu2;
}

/// @brief Test creating a simple menu structure of items
TEST_F(MenuTests, SimpleMenuStructure) {
  Menu *mainMenu = new Menu("Main Menu");
  Loco *loco0 = new Loco(123, LocoSource::LocoSourceEntry);
  loco0->setName("Loco 123");
  Loco *loco1 = new Loco(234, LocoSource::LocoSourceEntry);
  loco1->setName("Loco 234");
  Loco *loco2 = new Loco(555, LocoSource::LocoSourceEntry);
  mainMenu->addItem(new LocoMenuItem(loco0));
  mainMenu->addItem(new LocoMenuItem(loco1));
  mainMenu->addItem(new LocoMenuItem(loco2));

  // Validate each menu item has the correct auto generated index and name is
  // the Loco name
  int i = 0;
  for (LocoMenuItem *item =
           static_cast<LocoMenuItem *>(mainMenu->getFirstItem());
       item; item = static_cast<LocoMenuItem *>(item->getNext())) {
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
  for (BaseMenuItem *item = mainMenu->getFirstItem(); item;
       item = item->getNext()) {
    EXPECT_EQ(item->getIndex(), i);
    i++;
  }

  // Clean up
  delete mainMenu;
}
