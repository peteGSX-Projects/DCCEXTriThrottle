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

#ifndef EVENTLISTENER_H
#define EVENTLISTENER_H

#include "EventStructure.h"

/// @brief Class to extend for all other classes that need to respond to events
/// For example: Class <MyClass> : public EventListener () {}
/// The recommended approach is to implement a switch/case in the onEvent() implementation to handle valid event types
class EventListener {
public:
  /// @brief Virtual method to implement in the inheriting class to respond to events
  /// @param event The event to respond to
  virtual void onEvent(Event &event) = 0;
};

#endif // EVENTLISTENER_H
