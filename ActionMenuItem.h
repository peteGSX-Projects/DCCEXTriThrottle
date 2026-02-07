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

#ifndef ACTIONMENUITEM_H
#define ACTIONMENUITEM_H

#include "BaseMenuItem.h"
#include "EventStructure.h"

class ActionMenuItem : public BaseMenuItem {
public:
  /**
   * @brief Construct a new Action Menu Item object
   * @param name Name or title to be displayed in the menu
   * @param eventType EventType to be published when selecting this item
   * @param eventData EventData to be published when selecting this item
   */
  ActionMenuItem(const char *name, EventType eventType, EventData eventData);

  /**
   * @brief Get the Event Type associated with this item
   * @return EventType EventType that should be published with the event
   */
  EventType getEventType();

  /**
   * @brief Get the Event Data associated with this item
   * @return EventData EventData that should be published with the event
   */
  EventData getEventData();

  ~ActionMenuItem();

private:
  EventType _eventType;
  EventData _eventData;
};

#endif // ACTIONMENUITEM_H
