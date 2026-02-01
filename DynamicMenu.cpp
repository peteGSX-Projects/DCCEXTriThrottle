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

#include "DynamicMenu.h"
#include "LocoMenuItem.h"
#include "ActionMenuItem.h"
#include "src/DCCEXLoco.h"
#include "src/DCCEXTurnouts.h"
#include "src/DCCEXRoutes.h"

DynamicMenu::DynamicMenu(const char *name, Loco *firstLoco, int itemsPerPage, bool isProgmem)
    : Menu(name, itemsPerPage, isProgmem), _dynamicType(DynamicMenuType_Roster), _sourceData((void *)firstLoco),
      _tempItem(nullptr) {}

DynamicMenu::DynamicMenu(const char *name, Turnout *firstTurnout, int itemsPerPage, bool isProgmem)
    : Menu(name, itemsPerPage, isProgmem), _dynamicType(DynamicMenuType_Turnouts), _sourceData((void *)firstTurnout),
      _tempItem(nullptr) {}

DynamicMenu::DynamicMenu(const char *name, Route *firstRoute, int itemsPerPage, bool isProgmem)
    : Menu(name, itemsPerPage, isProgmem), _dynamicType(DynamicMenuType_Routes), _sourceData((void *)firstRoute),
      _tempItem(nullptr) {}

void *DynamicMenu::_getSourceItemAtIndex(int index) {
  void *current = _sourceData;

  for (int i = 0; i < index && current != nullptr; i++) {
    switch (_dynamicType) {
    case DynamicMenuType_Roster: {
      Loco *loco = (Loco *)current;
      current = (void *)loco->getNext();
      break;
    }
    case DynamicMenuType_Turnouts: {
      Turnout *turnout = (Turnout *)current;
      current = (void *)turnout->getNext();
      break;
    }
    case DynamicMenuType_Routes: {
      Route *route = (Route *)current;
      current = (void *)route->getNext();
      break;
    }
    }
  }

  return current;
}

BaseMenuItem *DynamicMenu::_createTempItemAtIndex(int index) {
  void *sourceItem = _getSourceItemAtIndex(index);

  if (sourceItem == nullptr)
    return nullptr;

  switch (_dynamicType) {
  case DynamicMenuType_Roster: {
    Loco *loco = (Loco *)sourceItem;
    return new LocoMenuItem(loco);
  }
  case DynamicMenuType_Turnouts: {
    Turnout *turnout = (Turnout *)sourceItem;
    return new ActionMenuItem(turnout->getName(), EventType::ToggleTurnout, EventData(turnout->getId()));
  }
  case DynamicMenuType_Routes: {
    Route *route = (Route *)sourceItem;
    EventType type = (route->getType() == RouteType::RouteTypeRoute) ? EventType::StartRoute : EventType::StartAutomation;
    return new ActionMenuItem(route->getName(), type, EventData(route->getId()));
  }
  }

  return nullptr;
}

BaseMenuItem *DynamicMenu::getItemByPageIndex(int index) {
  // Out of bounds check
  if (index < 0 || index >= getItemsPerPage())
    return nullptr;

  // Map index against current page and items per page
  int menuIndex = (getCurrentPage() * getItemsPerPage()) + index;

  // Create and return temporary item
  _tempItem = _createTempItemAtIndex(menuIndex);
  return _tempItem;
}

int DynamicMenu::getItemCount() {
  int count = 0;
  void *current = _sourceData;

  while (current != nullptr) {
    switch (_dynamicType) {
    case DynamicMenuType_Roster: {
      Loco *loco = (Loco *)current;
      current = (void *)loco->getNext();
      break;
    }
    case DynamicMenuType_Turnouts: {
      Turnout *turnout = (Turnout *)current;
      current = (void *)turnout->getNext();
      break;
    }
    case DynamicMenuType_Routes: {
      Route *route = (Route *)current;
      current = (void *)route->getNext();
      break;
    }
    }
    count++;
  }

  return count;
}

int DynamicMenu::getTotalPages() {
  int count = getItemCount();
  if (count == 0)
    return 1;
  return ((count + getItemsPerPage() - 1) / getItemsPerPage());
}

DynamicMenu::~DynamicMenu() {
  // Delete the temporary item if it exists
  if (_tempItem != nullptr) {
    delete _tempItem;
    _tempItem = nullptr;
  }
  // Don't delete the source data - it's owned elsewhere
}
