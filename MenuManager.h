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
   * @return true If Menu::getParent() is nullptr
   * @return false If Menu::getParent() is another Menu instance
   */
  bool isAtRootMenu();

  /**
   * @brief Destroy the Menu Manager object
   */
  ~MenuManager();

private:
  EventManager *_eventManager;
  Logger *_logger;
  Menu *_currentMenu;

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
};

#endif // MENUMANAGER_H