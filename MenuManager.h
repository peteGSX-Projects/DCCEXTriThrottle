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

#ifndef MENUMANAGER_H
#define MENUMANAGER_H

#include "EventManager.h"
#include "Menu.h"
#include "UserInputInterface.h"

class MenuManager {
public:
  /**
   * @brief Construct a new Menu Manager object
   * @param eventManager Pointer to the application EventManager instance
   * @param logger Pointer to the application Logger instance
   */
  MenuManager(EventManager *eventManager, Logger *logger);

  /**
   * @brief Call this once at startup to create the necessary static menu structure
   */
  void initialise();

  /**
   * @brief Respond to user input
   * @param inputEvent UserInputInterface::UserInputEvent
   */
  void handleUserInput(UserInputInterface::UserInputEvent inputEvent);

  /**
   * @brief Get the Current Menu object
   * @return Menu* Pointer to the current Menu instance
   */
  Menu *getCurrentMenu();

  /**
   * @brief Set the Current Menu object
   * @param menu Pointer to the Menu object
   */
  void setCurrentMenu(Menu *menu);

  /**
   * @brief Check if the MenuManager is currently at the root Menu instance
   * @return true If navigation is at the root of the stack (-1)
   * @return false If navigation is anywhere else in the stack
   */
  bool isAtRootMenu();

  /**
   * @brief Get the index of the Throttle instance that is being managed
   * @return int Throttle index, -1 when no throttle selected
   */
  int getActiveThrottleIndex();

  /**
   * @brief Set the Active Throttle Index
   * @param index Index of the throttle to manage, or -1 for none
   */
  void setActiveThrottleIndex(int index);

  /**
   * @brief Reset the menu navigation state to the root menu and no throttle context
   */
  void reset();

  /**
   * @brief Set the Root Menu object
   * @param menu Pointer to the root Menu instance
   */
  void setRootMenu(Menu *menu);

  /**
   * @brief Get the Root Menu object
   * @return Menu* Pointer to the root Menu instance
   */
  Menu *getRootMenu();

  /**
   * @brief Create the Roster Menu from the DCC-EX roster
   * @param roster Pointer to the first Loco in the roster
   */
  void createRosterMenu(Loco *roster);

  /**
   * @brief Destroy the Menu Manager object
   */
  ~MenuManager();

private:
  // Structure to facilitate menu navigation keeping throttle context where needed
  struct NavigationNode {
    Menu *menu;
    int throttleIndex;
  };

  EventManager *_eventManager;
  Logger *_logger;
  Menu *_currentMenu;
  int _activeThrottleIndex;
  static const int _MAX_MENU_DEPTH = 6;
  static const int _MAX_MANAGED_MENUS = 20;
  NavigationNode _history[_MAX_MENU_DEPTH];
  Menu *_allManagedMenus[_MAX_MANAGED_MENUS];
  int _historyIndex;
  Menu *_rootMenu;
  Menu *_rosterMenu;
  Menu *_turnoutMenu;
  Menu *_turntableMenu;
  Menu *_routeMenu;
  Menu *_automationMenu;
  int _menuCount;

  /**
   * @brief Handle navigating to the parent item
   */
  void _handleBack();

  /**
   * @brief Handle navigating to the next page
   */
  void _handleNextPage();

  /**
   * @brief Handle selection of the item
   * @param digit Number of the key pressed by the user
   */
  void _handleSelection(int digit);

  /**
   * @brief Push the current menu context and throttle index to the stack
   * @param menu Pointer to the current menu
   * @param index Index of the current throttle
   */
  void _push(Menu *menu, int index);

  /**
   * @brief Pop the latest NavigationNode from the stack
   * @return NavigationNode Pointer to the Menu and the throttle index
   */
  NavigationNode _pop();

  /**
   * @brief Helper method to create and register a menu
   * @param name Menu name
   * @return Menu* Pointer to the created Menu instance
   */
  Menu *_createManagedMenu(const char *name);

  /**
   * @brief Creates a throttle menu for the index provided
   * @param index Index this throttle is associated with, will be incremented for the name
   */
  Menu *_createThrottleMenu(int index);

  /**
   * @brief Sets up items in the Tracks menu
   * @param tracksMenu Pointer to the Tracks menu instance
   */
  void _setupTracksMenu(Menu *tracksMenu);
};

#endif // MENUMANAGER_H