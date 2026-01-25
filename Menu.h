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

#ifndef MENU_H
#define MENU_H

#include "ActionMenuItem.h"
#include "BaseMenuItem.h"
#include "LocoMenuItem.h"
#include "SubMenuItem.h"
#include "ThrottleMenuItem.h"

/**
 * @brief Menu class supporting nested menus.
 */
class Menu {
public:
  /**
   * @brief Construct a new Menu object
   * @param name Name or label for this menu
   * @param itemsPerPage Override the items per page default of 10 if necessary
   */
  Menu(const char *name, int itemsPerPage = 10);

  /**
   * @brief Get the Name of this menu
   * @return const char* Pointer to the char array name
   */
  const char *getName();

  /**
   * @brief Add a menu item to this menu and automatically set its index
   * @param item Pointer to the menu item to add
   */
  void addItem(BaseMenuItem *item);

  /**
   * @brief Get the First Item object
   * @return BaseMenuItem* Pointer to the first menu item
   */
  BaseMenuItem *getFirstItem();

  /**
   * @brief Get the Current Page
   * @return int Index of the current page
   */
  int getCurrentPage();

  /**
   * @brief Set the Current Page
   * @param page Index of the page
   */
  void setCurrentPage(int page);

  /**
   * @brief Get the Total Pages
   * @return int Count of pages of items
   */
  int getTotalPages();

  /**
   * @brief Advance to the next page, wraps to first page from the last
   */
  void nextPage();

  /**
   * @brief Get the Item Count
   * @return int Total number of items in this Menu
   */
  int getItemCount();

  /**
   * @brief Get the Items Per Page
   * @return int Items per page to display for this menu
   */
  int getItemsPerPage();

  /**
   * @brief Get the Item By Page Index, uses user select and current page to determine index
   * @param index Index of the item selected by the user
   * @return BaseMenuItem* Pointer to the selected item
   */
  BaseMenuItem *getItemByPageIndex(int index);

  /**
   * @brief Clears all related menu items and resets menu state
   */
  void clearItems();

  /**
   * @brief Destroy the Menu object, also deletes all associated items
   */
  ~Menu();

private:
  char *_name;              /** Name or label of this menu */
  BaseMenuItem *_firstItem; /** Pointer to the first menu item in the list */
  int _nextItemIndex;       /** Auto increment index of next item added */
  int _currentPage;         /** Index of the current selected page */
  int _itemsPerPage;        /** Number of items per page for this menu */
};

#endif // MENU_H
