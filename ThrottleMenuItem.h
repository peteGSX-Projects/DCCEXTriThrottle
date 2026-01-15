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

#ifndef THROTTLEMENUITEM_H
#define THROTTLEMENUITEM_H

#include "SubMenuItem.h"

class ThrottleMenuItem : public SubMenuItem {
public:
  /**
   * @brief Construct a new Throttle Menu Item object
   * @param menu Pointer to the Menu instance associated with this item
   * @param throttleIndex Index of the throttle this menu manages
   * @param name Optional name override for the menu, otherwise use Menu name
   */
  ThrottleMenuItem(Menu *menu, int throttleIndex, const char *name = nullptr);

  /**
   * @brief Get the Throttle Index
   * @return int Index of the throttle this menu manages
   */
  int getThrottleIndex();

  /**
   * @brief Get the Item Type object, overrides BaseMenuItem method
   * @return MenuItemType Always returns MenuItemType::ThrottleMenuType
   */
  MenuItemType getItemType() override;

  /**
   * @brief Destroy the Throttle Menu Item object
   */
  ~ThrottleMenuItem();

private:
  int _throttleIndex;
};

#endif // THROTTLEMENUITEM_H
