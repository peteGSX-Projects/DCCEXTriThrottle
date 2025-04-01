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

#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

#include "EventListener.h"
#include "EventStructure.h"
#include "Logger.h"

/// @brief Class to centrally manage all events for the application
/// Exposes methods to register subscribers that need to respond to events, and
/// to publish events Dynamically manages a linked list of subscribers to be
/// notified when a relevant event is published Subscribers must extend the
/// EventListener class and implement the onEvent() virtual method provided
class EventManager {
public:
  /// @brief Constructor for the instance
  EventManager();

  /// @brief Method to subscribe a listener to the specified event type
  /// @param eventLister Pointer to a class instance that extends the
  /// EventListener class
  /// @param eventType Valid EventType the subscriber needs to be notified about
  void subscribe(EventListener *eventListener, EventType eventType);

  /// @brief Method to unsubscribe the listener from the specified event type
  /// @param eventListener Pointer to a class instance that extends the
  /// EventListener class
  /// @param eventType Valid EventType to unsubscribe from
  void unsubscribe(EventListener *eventListener, EventType eventType);

  /// @brief Method to check if the specified listener is subscribed to the
  /// specified event type
  /// @param eventListener Pointer to a class instance that extends the
  /// EventListener class
  /// @param eventType Valid EventType to check subscription for
  /// @return true|false
  bool isSubscribed(EventListener *eventListener, EventType eventType);

  /// @brief Method to publish an event
  /// @param eventType A valid EventType
  /// @param eventData Valid EventData
  void publish(EventType eventType, EventData eventData);

  /// @brief Destructor for the EventManager
  ~EventManager();

private:
  /// @brief EventSubscriber structure to maintain the linked list of
  /// subscribers
  struct EventSubscriber {
    EventListener *eventListener; // Pointer to the class instance extending EventListener
    EventType eventType;          // EventType to listen for
    EventSubscriber *next;        // Pointer to the next subscriber in the list

    /// @brief Constructor for each event listener
    /// @param eventListener Pointer to the class instance extending
    /// EventListener
    /// @param eventType EventType to listen for
    EventSubscriber(EventListener *eventListener, EventType eventType)
        : eventListener(eventListener), eventType(eventType), next(nullptr) {}
  };

  EventSubscriber *_firstEventSubscriber; // Pointer to the first subscriber in the list
};

#endif // EVENTMANAGER_H
