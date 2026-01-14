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

#include "MenuManager.h"
#include "test/mocks/MockEventListener.h"
#include <gtest/gtest.h>

using namespace testing;

class MenuManagerTests : public Test {
protected:
  EventManager *eventManager;
  MenuManager *menuManager;

  void SetUp() override {
    eventManager = new EventManager();
    menuManager = new MenuManager(eventManager, nullptr);
  }

  void TearDown() override {
    delete menuManager;
    delete eventManager;
  }
};

/**
 * @brief Test selection of a sub menu changes the current menu
 */
TEST_F(MenuManagerTests, TestMenuSelectionToSubMenu) {
  // Setup a nested menu
  Menu *root = new Menu("Root");
  Menu *subMenu = new Menu("Submenu");
  root->addItem(new SubMenuItem(subMenu));

  // Set current to the root to start
  menuManager->setCurrentMenu(root);

  // Submenu is at index 0, simulate a keypress
  UserInputInterface::UserInputEvent event = {'0', UserInputInterface::UserInputAction::Pressed};
  menuManager->handleUserInput(event);

  // Verify the current menu is now Submenu
  EXPECT_EQ(menuManager->getCurrentMenu(), subMenu);
  EXPECT_STREQ(menuManager->getCurrentMenu()->getName(), "Submenu");

  // Cleanup
  delete subMenu;
  delete root;
}

/**
 * @brief Test using the '*' key navigates from a nested menu to the parent menu
 */
TEST_F(MenuManagerTests, TestBackNavigation) {
  // Setup a nested menu
  Menu *root = new Menu("Root");
  Menu *subMenu = new Menu("Submenu");
  root->addItem(new SubMenuItem(subMenu));

  // Set current to the Submenu to start
  menuManager->setCurrentMenu(subMenu);

  // Setup a mock listener and subscribe to MenuRefreshRequired
  MockEventListener *orchestrator = new MockEventListener();
  eventManager->subscribe(orchestrator, EventType::MenuRefreshRequired);

  // Setup call expectation
  EXPECT_CALL(
      *orchestrator,
      onEvent(::testing::AllOf(
          ::testing::Field(&Event::eventType, EventType::MenuRefreshRequired),
          ::testing::Field(&Event::eventData, ::testing::Field(&EventData::dataType, EventData::DataType::NoneData)))))
      .Times(1);

  // Submenu is at index 0, simulate a keypress
  UserInputInterface::UserInputEvent event = {'*', UserInputInterface::UserInputAction::Pressed};
  menuManager->handleUserInput(event);

  // Verify the current menu is now root
  EXPECT_EQ(menuManager->getCurrentMenu(), root);
  EXPECT_STREQ(menuManager->getCurrentMenu()->getName(), "Root");

  // Cleanup
  delete orchestrator;
  delete subMenu;
  delete root;
}

/**
 * @brief Test pressing '*' at the root menu publishes an ExitMenu event
 */
TEST_F(MenuManagerTests, TestExitMenu) {
  // Setup a mock listener and subscribe to ExitMenu
  MockEventListener *orchestrator = new MockEventListener();
  eventManager->subscribe(orchestrator, EventType::ExitMenu);

  // Setup the root menu
  Menu *root = new Menu("Root");
  menuManager->setCurrentMenu(root);

  // Setup expectation of the event with no data
  EXPECT_CALL(
      *orchestrator,
      onEvent(::testing::AllOf(
          ::testing::Field(&Event::eventType, EventType::ExitMenu),
          ::testing::Field(&Event::eventData, ::testing::Field(&EventData::dataType, EventData::DataType::NoneData)))))
      .Times(1);

  // Simulate key press of '*'
  UserInputInterface::UserInputEvent event = {'*', UserInputInterface::UserInputAction::Pressed};
  menuManager->handleUserInput(event);

  // Clean up
  delete orchestrator;
  delete root;
}

/**
 * @brief Test selecting a Loco menu item publishes the loco via the event
 */
TEST_F(MenuManagerTests, TestSelectLoco) {
  // Setup a mock listener and subscribe to LocoSelected
  MockEventListener *orchestrator = new MockEventListener();
  eventManager->subscribe(orchestrator, EventType::LocoSelected);

  // Create the dummy Loco and add to a menu and menu item
  Loco *loco = new Loco(3, LocoSource::LocoSourceEntry);
  Menu *menu = new Menu("Roster");
  LocoMenuItem *item = new LocoMenuItem(loco);
  menu->addItem(item);
  menuManager->setCurrentMenu(menu);

  // Setup expectation of the event with the Loco data
  EXPECT_CALL(
      *orchestrator,
      onEvent(::testing::AllOf(
          ::testing::Field(&Event::eventType, EventType::LocoSelected),
          ::testing::Field(&Event::eventData, ::testing::Field(&EventData::dataType, EventData::DataType::LocoData)))))
      .Times(1);

  // Simulate key press of '0'
  UserInputInterface::UserInputEvent event = {'0', UserInputInterface::UserInputAction::Pressed};
  menuManager->handleUserInput(event);

  // Clean up
  delete menu;
  delete loco;
  delete orchestrator;
}
