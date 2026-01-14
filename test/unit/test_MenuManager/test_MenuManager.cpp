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
    eventManager = new EventManager(nullptr);
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
 * @brief Test the navigation stack works forward and backwards
 */
TEST_F(MenuManagerTests, TestStackBasedNavigation) {
  // Create menu structure and start at root
  Menu *root = new Menu("Root");
  Menu *subMenu = new Menu("Submenu");
  root->addItem(new SubMenuItem(subMenu));
  menuManager->setCurrentMenu(root);

  // Simulate user input '0' to select submenu
  menuManager->handleUserInput({'0', UserInputInterface::UserInputAction::Pressed});
  EXPECT_EQ(menuManager->getCurrentMenu(), subMenu);

  // Simulate back with '*'
  menuManager->handleUserInput({'*', UserInputInterface::UserInputAction::Pressed});
  EXPECT_EQ(menuManager->getCurrentMenu(), root);

  // Clean up
  delete subMenu;
  delete root;
}

/**
 * @brief Test navigation maintains appropriate throttle index/context
 */
TEST_F(MenuManagerTests, TestThrottleContextInNavigation) {
  // Create menu structure and start at root
  Menu *root = new Menu("Root");
  Menu *throttleMenu = new Menu("Throttle 2");
  Menu *throttleSubMenu = new Menu("Throttle Submenu");
  root->addItem(new ThrottleMenuItem(throttleMenu, 1));
  throttleMenu->addItem(new SubMenuItem(throttleSubMenu));
  menuManager->setCurrentMenu(root);

  // Simulate '0' twice to get to the throttle submenu
  menuManager->handleUserInput({'0', UserInputInterface::UserInputAction::Pressed});
  menuManager->handleUserInput({'0', UserInputInterface::UserInputAction::Pressed});

  // Throttle index should be 1
  EXPECT_EQ(menuManager->getActiveThrottleIndex(), 1);

  // Back to throttle menu, index should still be 1
  menuManager->handleUserInput({'*', UserInputInterface::UserInputAction::Pressed});
  EXPECT_EQ(menuManager->getActiveThrottleIndex(), 1);

  // Back to root menu, index should now be unset (-1)
  menuManager->handleUserInput({'*', UserInputInterface::UserInputAction::Pressed});
  EXPECT_EQ(menuManager->getActiveThrottleIndex(), -1);

  // Clean up
  delete throttleSubMenu;
  delete throttleMenu;
  delete root;
}

/**
 * @brief Test a reset() clears the MenuManager navigation stack
 */
TEST_F(MenuManagerTests, TestResetNavigationState) {
  // Create a menu structure
  Menu *root = new Menu("Root");
  Menu *throttleMenu = new Menu("Throttle 3");
  root->addItem(new ThrottleMenuItem(throttleMenu, 2));
  menuManager->setCurrentMenu(root);
  menuManager->setRootMenu(root);

  // Navigate to the throttle menu and ensure state is as expected
  menuManager->handleUserInput({'0', UserInputInterface::UserInputAction::Pressed});
  ASSERT_EQ(menuManager->getCurrentMenu(), throttleMenu);
  ASSERT_EQ(menuManager->getActiveThrottleIndex(), 2);

  // Reset navigation state and ensure state is reset
  menuManager->reset();
  EXPECT_EQ(menuManager->getActiveThrottleIndex(), -1);
  EXPECT_TRUE(menuManager->isAtRootMenu());
  EXPECT_EQ(menuManager->getCurrentMenu(), root);

  // Clean up
  delete throttleMenu;
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
  EXPECT_CALL(*orchestrator,
              onEvent(AllOf(Field(&Event::eventType, EventType::ExitMenu),
                            Field(&Event::eventData, Field(&EventData::dataType, EventData::DataType::NoneData)))))
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

  // Set the active throttle index
  menuManager->setActiveThrottleIndex(1);

  // Setup expectation of the event with the Loco data
  EXPECT_CALL(*orchestrator,
              onEvent(AllOf(Field(&Event::eventType, EventType::LocoSelected),
                            Field(&Event::eventData, Field(&EventData::dataType, EventData::DataType::SelectLocoData)),
                            Field(&Event::eventData,
                                  Field(&EventData::selectLocoValue,
                                        AllOf(Field(&SelectLoco::loco, loco), Field(&SelectLoco::throttleIndex, 1)))))))
      .Times(1);

  // Simulate key press of '0'
  UserInputInterface::UserInputEvent event = {'0', UserInputInterface::UserInputAction::Pressed};
  menuManager->handleUserInput(event);

  // Clean up
  delete menu;
  delete loco;
  delete orchestrator;
}

/**
 * @brief Test selecting a Loco menu with no throttle context does not publish an event
 */
TEST_F(MenuManagerTests, TestViewOnlyRoster) {
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
  EXPECT_CALL(*orchestrator, onEvent(_)).Times(0);

  // Simulate key press of '0'
  UserInputInterface::UserInputEvent event = {'0', UserInputInterface::UserInputAction::Pressed};
  menuManager->handleUserInput(event);

  // Clean up
  delete menu;
  delete loco;
  delete orchestrator;
}
