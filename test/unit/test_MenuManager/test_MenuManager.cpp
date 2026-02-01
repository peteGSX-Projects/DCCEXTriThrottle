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
#include "test/mocks/DCCEXTestHelpers.h"
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

/**
 * @brief Test calling initialise() creates the initial static menu structure
 */
TEST_F(MenuManagerTests, TestInitialiseCreatesStructure) {
  // Call initialise
  menuManager->initialise();

  // Expect initially current menu to be Main Menu
  EXPECT_STREQ(menuManager->getCurrentMenu()->getName(), "Main Menu");

  // '0' selects first throttle
  menuManager->handleUserInput({'0', UserInputInterface::UserInputAction::Pressed});
  EXPECT_STREQ(menuManager->getCurrentMenu()->getName(), "Throttle 1");
  EXPECT_EQ(menuManager->getActiveThrottleIndex(), 0);

  // '*' Back to main
  menuManager->handleUserInput({'*', UserInputInterface::UserInputAction::Pressed});
  EXPECT_STREQ(menuManager->getCurrentMenu()->getName(), "Main Menu");
  EXPECT_EQ(menuManager->getActiveThrottleIndex(), -1);

  // '1' selects second throttle
  menuManager->handleUserInput({'1', UserInputInterface::UserInputAction::Pressed});
  EXPECT_STREQ(menuManager->getCurrentMenu()->getName(), "Throttle 2");
  EXPECT_EQ(menuManager->getActiveThrottleIndex(), 1);

  // '*' Back to main
  menuManager->handleUserInput({'*', UserInputInterface::UserInputAction::Pressed});
  EXPECT_STREQ(menuManager->getCurrentMenu()->getName(), "Main Menu");
  EXPECT_EQ(menuManager->getActiveThrottleIndex(), -1);

  // '2' selects third throttle
  menuManager->handleUserInput({'2', UserInputInterface::UserInputAction::Pressed});
  EXPECT_STREQ(menuManager->getCurrentMenu()->getName(), "Throttle 3");
  EXPECT_EQ(menuManager->getActiveThrottleIndex(), 2);

  // '*' Back to main
  menuManager->handleUserInput({'*', UserInputInterface::UserInputAction::Pressed});
  EXPECT_STREQ(menuManager->getCurrentMenu()->getName(), "Main Menu");
  EXPECT_EQ(menuManager->getActiveThrottleIndex(), -1);

  // '3' selects turnouts
  menuManager->handleUserInput({'3', UserInputInterface::UserInputAction::Pressed});
  EXPECT_STREQ(menuManager->getCurrentMenu()->getName(), "Turnouts");
  EXPECT_EQ(menuManager->getActiveThrottleIndex(), -1);

  // '*' Back to main
  menuManager->handleUserInput({'*', UserInputInterface::UserInputAction::Pressed});
  EXPECT_STREQ(menuManager->getCurrentMenu()->getName(), "Main Menu");
  EXPECT_EQ(menuManager->getActiveThrottleIndex(), -1);

  // '4' selects turntables
  menuManager->handleUserInput({'4', UserInputInterface::UserInputAction::Pressed});
  EXPECT_STREQ(menuManager->getCurrentMenu()->getName(), "Turntables");
  EXPECT_EQ(menuManager->getActiveThrottleIndex(), -1);

  // '*' Back to main
  menuManager->handleUserInput({'*', UserInputInterface::UserInputAction::Pressed});
  EXPECT_STREQ(menuManager->getCurrentMenu()->getName(), "Main Menu");
  EXPECT_EQ(menuManager->getActiveThrottleIndex(), -1);

  // '5' selects routes
  menuManager->handleUserInput({'5', UserInputInterface::UserInputAction::Pressed});
  EXPECT_STREQ(menuManager->getCurrentMenu()->getName(), "Routes");
  EXPECT_EQ(menuManager->getActiveThrottleIndex(), -1);

  // '*' Back to main
  menuManager->handleUserInput({'*', UserInputInterface::UserInputAction::Pressed});
  EXPECT_STREQ(menuManager->getCurrentMenu()->getName(), "Main Menu");
  EXPECT_EQ(menuManager->getActiveThrottleIndex(), -1);

  // '6' views roster
  menuManager->handleUserInput({'6', UserInputInterface::UserInputAction::Pressed});
  EXPECT_STREQ(menuManager->getCurrentMenu()->getName(), "Roster");
  EXPECT_EQ(menuManager->getActiveThrottleIndex(), -1);

  // '*' Back to main
  menuManager->handleUserInput({'*', UserInputInterface::UserInputAction::Pressed});
  EXPECT_STREQ(menuManager->getCurrentMenu()->getName(), "Main Menu");
  EXPECT_EQ(menuManager->getActiveThrottleIndex(), -1);

  // '7' manages tracks
  menuManager->handleUserInput({'7', UserInputInterface::UserInputAction::Pressed});
  EXPECT_STREQ(menuManager->getCurrentMenu()->getName(), "Tracks");
  EXPECT_EQ(menuManager->getActiveThrottleIndex(), -1);

  // '*' Back to main
  menuManager->handleUserInput({'*', UserInputInterface::UserInputAction::Pressed});
  EXPECT_STREQ(menuManager->getCurrentMenu()->getName(), "Main Menu");
  EXPECT_EQ(menuManager->getActiveThrottleIndex(), -1);

  // '8' shows system info
  menuManager->handleUserInput({'8', UserInputInterface::UserInputAction::Pressed});
  EXPECT_STREQ(menuManager->getCurrentMenu()->getName(), "System");
  EXPECT_EQ(menuManager->getActiveThrottleIndex(), -1);

  // '*' Back to main
  menuManager->handleUserInput({'*', UserInputInterface::UserInputAction::Pressed});
  EXPECT_STREQ(menuManager->getCurrentMenu()->getName(), "Main Menu");
  EXPECT_EQ(menuManager->getActiveThrottleIndex(), -1);
}

/**
 * @brief Test a throttle menu contains the correct menus and throttle context
 */
TEST_F(MenuManagerTests, TestThrottleMenuStructure) {
  // Call initialise
  menuManager->initialise();

  // Navigate to throttle 2 (index 1) and validate
  menuManager->handleUserInput({'1', UserInputInterface::UserInputAction::Pressed});
  EXPECT_STREQ(menuManager->getCurrentMenu()->getName(), "Throttle 2");
  EXPECT_EQ(menuManager->getActiveThrottleIndex(), 1);

  // Index item 0 should be Select Loco which is the roster menu
  BaseMenuItem *item0 = menuManager->getCurrentMenu()->getItemByPageIndex(0);
  ASSERT_NE(item0, nullptr);
  EXPECT_STREQ(item0->getName(), "Select Loco");
  // Index item 1 should be Enter Address
  BaseMenuItem *item1 = menuManager->getCurrentMenu()->getItemByPageIndex(1);
  ASSERT_NE(item1, nullptr);
  EXPECT_STREQ(item1->getName(), "Enter Address");

  // First item should be the roster
  menuManager->handleUserInput({'0', UserInputInterface::UserInputAction::Pressed});
  EXPECT_STREQ(menuManager->getCurrentMenu()->getName(), "Roster");
  EXPECT_EQ(menuManager->getActiveThrottleIndex(), 1);
}

/**
 * @brief Test createRoster() creates the roster menu from the protocol roster object
 */
TEST_F(MenuManagerTests, TestCreateRosterMenu) {
  // Initialise menus
  menuManager->initialise();

  // Create a DCCEXProtocol instance and create the dummy roster
  DCCEXProtocol *client = new DCCEXProtocol;
  DCCEXTestHelpers::createMockRoster(client);
  menuManager->createRosterMenu(client->roster);

  // Ensure roster is accessible via Throttle 1 via two '0' presses
  menuManager->handleUserInput({'0', UserInputInterface::UserInputAction::Pressed});
  menuManager->handleUserInput({'0', UserInputInterface::UserInputAction::Pressed});
  EXPECT_STREQ(menuManager->getCurrentMenu()->getName(), "Roster");
  ASSERT_NE(menuManager->getCurrentMenu()->getItemByPageIndex(0), nullptr);
  EXPECT_STREQ(menuManager->getCurrentMenu()->getItemByPageIndex(0)->getName(), "Loco1");
  EXPECT_STREQ(menuManager->getCurrentMenu()->getItemByPageIndex(4)->getName(), "Loco5");

  // Ensure roster is accessible via main menu, '*' back to main then '6'
  menuManager->handleUserInput({'*', UserInputInterface::UserInputAction::Pressed});
  menuManager->handleUserInput({'*', UserInputInterface::UserInputAction::Pressed});
  menuManager->handleUserInput({'6', UserInputInterface::UserInputAction::Pressed});
  EXPECT_STREQ(menuManager->getCurrentMenu()->getName(), "Roster");
  EXPECT_STREQ(menuManager->getCurrentMenu()->getItemByPageIndex(0)->getName(), "Loco1");
  EXPECT_STREQ(menuManager->getCurrentMenu()->getItemByPageIndex(4)->getName(), "Loco5");

  // Clean up
  delete client;
}

/**
 * @brief Test selecting toggle track power from the menu publishes the event
 */
TEST_F(MenuManagerTests, TestToggleTrackPowerPublishesEvent) {
  // Setup a mock listener and subscribe to ToggleTrackPower
  MockEventListener *orchestrator = new MockEventListener();
  eventManager->subscribe(orchestrator, EventType::ToggleTrackPower);

  // Initialise menus
  menuManager->initialise();

  // Assert/navigate to tracks menu and assert that first item is Toggle Power
  menuManager->handleUserInput({'7', UserInputInterface::UserInputAction::Pressed});
  ASSERT_STREQ(menuManager->getCurrentMenu()->getName(), "Tracks");
  ASSERT_NE(menuManager->getCurrentMenu()->getItemByPageIndex(0), nullptr);
  ASSERT_STREQ(menuManager->getCurrentMenu()->getItemByPageIndex(0)->getName(), "Toggle Power");

  // Setup expectation of the event with no data
  EXPECT_CALL(*orchestrator,
              onEvent(AllOf(Field(&Event::eventType, EventType::ToggleTrackPower),
                            Field(&Event::eventData, Field(&EventData::dataType, EventData::DataType::NoneData)))))
      .Times(1);

  // Select item 0
  menuManager->handleUserInput({'0', UserInputInterface::UserInputAction::Pressed});

  // Clean up
  delete orchestrator;
}

/**
 * @brief Test createTurnoutMenu() creates the turnout menu from the protocol turnout list
 */
TEST_F(MenuManagerTests, TestCreateTurnoutMenu) {
  // Initialise menus
  menuManager->initialise();

  // Create a DCCEXProtocol instance and create the dummy turnout list
  DCCEXProtocol *client = new DCCEXProtocol;
  DCCEXTestHelpers::createMockTurnoutList(client);

  // Call createRosterMenu() with the first entry
  menuManager->createTurnoutMenu(client->turnouts);

  // Ensure turnout menu is accessible via main menu, '3'
  menuManager->handleUserInput({'3', UserInputInterface::UserInputAction::Pressed});
  EXPECT_STREQ(menuManager->getCurrentMenu()->getName(), "Turnouts");
  ASSERT_NE(menuManager->getCurrentMenu()->getItemByPageIndex(0), nullptr);
  EXPECT_STREQ(menuManager->getCurrentMenu()->getItemByPageIndex(0)->getName(), "Turnout1");
  EXPECT_STREQ(menuManager->getCurrentMenu()->getItemByPageIndex(4)->getName(), "Turnout5");

  // Clean up
  delete client;
}

/**
 * @brief Test createRouteMenus() creates both the Routes and Automation menus from the route list
 */
TEST_F(MenuManagerTests, TestCreateRouteAutomationMenus) {
  // Initialise menus
  menuManager->initialise();

  // Create a protocol instance and the dummy route list
  DCCEXProtocol *client = new DCCEXProtocol;
  DCCEXTestHelpers::createMockRouteList(client);

  // Call createRouteMenus() with the first entry
  menuManager->createRouteMenus(client->routes);

  // Ensure route menu is accessible via main menu, '5'
  menuManager->handleUserInput({'5', UserInputInterface::UserInputAction::Pressed});
  EXPECT_STREQ(menuManager->getCurrentMenu()->getName(), "Routes");
  ASSERT_NE(menuManager->getCurrentMenu()->getFirstItem(), nullptr);
  EXPECT_STREQ(menuManager->getCurrentMenu()->getFirstItem()->getName(), "Route1");
  EXPECT_STREQ(menuManager->getCurrentMenu()->getItemByPageIndex(2)->getName(), "Route3");

  // Ensure automations are accessible via a throttle, '*' back to main then '0' and '2'
  menuManager->handleUserInput({'*', UserInputInterface::UserInputAction::Pressed});
  menuManager->handleUserInput({'0', UserInputInterface::UserInputAction::Pressed});

  // Item 2 should be Automations
  ASSERT_NE(menuManager->getCurrentMenu()->getItemByPageIndex(2), nullptr);
  EXPECT_STREQ(menuManager->getCurrentMenu()->getItemByPageIndex(2)->getName(), "Automations");

  // Select and make sure they're there
  menuManager->handleUserInput({'2', UserInputInterface::UserInputAction::Pressed});
  ASSERT_NE(menuManager->getCurrentMenu()->getFirstItem(), nullptr);
  EXPECT_STREQ(menuManager->getCurrentMenu()->getFirstItem()->getName(), "Automation1");
  EXPECT_STREQ(menuManager->getCurrentMenu()->getItemByPageIndex(2)->getName(), "Automation3");

  // Clean up
  delete client;
}
