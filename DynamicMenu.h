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

#ifndef DYNAMICMENU_H
#define DYNAMICMENU_H

#include "Menu.h"

// Forward declarations to avoid circular dependencies
class Loco;
class Turnout;
class Route;

/**
 * @brief DynamicMenu extends Menu to source items on-demand from linked lists
 * instead of pre-creating MenuItem objects. This saves significant SRAM by
 * eliminating the need to instantiate MenuItem objects for every roster item,
 * turnout, route, etc.
 *
 * Usage: Create DynamicMenu with the source data, and it will generate virtual
 * menu items when accessed via getItemByPageIndex().
 */
class DynamicMenu : public Menu {
public:
  /// @brief Menu type enum for DynamicMenu
  enum DynamicMenuType {
    DynamicMenuType_Roster,    // Sources from Loco* linked list
    DynamicMenuType_Turnouts,  // Sources from Turnout* linked list
    DynamicMenuType_Routes,    // Sources from Route* linked list
  };

  /// @brief Constructor for Roster dynamic menu
  /// @param name Name of this menu
  /// @param firstLoco Pointer to the first Loco in the roster list
  DynamicMenu(const char *name, Loco *firstLoco, int itemsPerPage = 10, bool isProgmem = false);

  /// @brief Constructor for Turnout dynamic menu
  /// @param name Name of this menu
  /// @param firstTurnout Pointer to the first Turnout in the turnout list
  DynamicMenu(const char *name, Turnout *firstTurnout, int itemsPerPage = 10, bool isProgmem = false);

  /// @brief Constructor for Route dynamic menu
  /// @param name Name of this menu
  /// @param firstRoute Pointer to the first Route in the route list
  DynamicMenu(const char *name, Route *firstRoute, int itemsPerPage = 10, bool isProgmem = false);

  /// @brief Get the Item By Page Index - generates a temporary MenuItem on-demand
  /// @param index Index of the item selected by the user
  /// @return BaseMenuItem* Pointer to a dynamically generated menu item (caller must delete)
  BaseMenuItem *getItemByPageIndex(int index) override;

  /// @brief Get the total number of items in this dynamic menu
  /// @return int Count of available items from the source data
  int getItemCount() override;

  /// @brief Get total pages based on actual item count
  /// @return int Count of pages
  int getTotalPages() override;

  /// @brief Destructor
  ~DynamicMenu();

private:
  DynamicMenuType _dynamicType;
  void *_sourceData;          // Pointer to Loco*, Turnout*, or Route* depending on type
  mutable BaseMenuItem *_tempItem;  // Temporary item created for getItemByPageIndex

  /// @brief Get the Nth item from the source linked list
  /// @param index Index to retrieve
  /// @return void* Pointer to the source object (Loco*, Turnout*, or Route*)
  void *_getSourceItemAtIndex(int index);

  /// @brief Create a temporary MenuItem from source data at given index
  /// @param index Index to create item for
  /// @return BaseMenuItem* Temporary item (must be deleted by caller)
  BaseMenuItem *_createTempItemAtIndex(int index);
};

#endif // DYNAMICMENU_H
