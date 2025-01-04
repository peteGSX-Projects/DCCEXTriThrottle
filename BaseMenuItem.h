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

#ifndef BASEMENUITEM_H
#define BASEMENUITEM_H

#include "Logger.h"
#include <Arduino.h>

/// @brief Types of menu items - used to determine how to deal with these items
enum MenuItemType { LocoType, SubMenuType };

/**
 * @brief BaseMenuItem is designed to be extended by other menu item classes as
 * part of a menu system.
 */
class BaseMenuItem {
public:
  /// @brief Constructor for this BaseMenuItem
  /// @param name Name or label for this item
  /// @param itemType The MenuItemType of this item
  BaseMenuItem(const char *name, MenuItemType itemType);

  /// @brief Set the index of this item
  /// @param index Index
  void setIndex(int index);

  /// @brief Get the index of this item
  /// @return Index
  int getIndex();

  /// @brief Get the name or label for this item
  /// @return Name or label
  const char *getName();

  /// @brief Set the next item in the linked list
  /// @param next Pointer to the next item
  void setNext(BaseMenuItem *next);

  /// @brief Get the next item in the linked list
  /// @return Pointer to the next item
  BaseMenuItem *getNext();

  /// @brief Set the Logger instance to use
  /// @param logger Pointer to the Logger instance
  void setLogger(Logger *logger);

  /// @brief Get the type of this menu item
  /// @return MenuItemType
  MenuItemType getItemType();

  /// @brief Virtual method to assist polymorphic destructor clean up
  /// @return Pointer to the polymorphic instance inheriting from this class
  virtual BaseMenuItem *clone() const = 0;

  /// @brief Destructor for this item - virtual to support polymorphic clean up
  virtual ~BaseMenuItem();

protected:
  char *_name;            /** The name or label of this menu item */
  int _index;             /** The index of this item */
  BaseMenuItem *_next;    /** The next item in the linked list */
  Logger *_logger;        /** Logger instance to use */
  MenuItemType _itemType; /** The MenuItemType for this item */
};

#endif // BASEMENUITEM_H
