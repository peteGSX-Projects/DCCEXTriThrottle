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

#include "EventManager.h"

EventManager::EventManager(Logger *logger) : _logger(logger), _firstEventSubscriber(nullptr) {}

void EventManager::subscribe(EventListener *eventListener, EventType eventType) {
  if (eventListener == nullptr)
    return;
  EventSubscriber *newSubscriber = new EventSubscriber(eventListener, eventType);
  if (_firstEventSubscriber == nullptr) {
    // If we don't have a first subscriber, this is it
    _firstEventSubscriber = newSubscriber;
  } else {
    // Otherwise traverse the list and add it to the end
    EventSubscriber *currentEventSubscriber = _firstEventSubscriber;
    while (currentEventSubscriber->next != nullptr) {
      currentEventSubscriber = currentEventSubscriber->next;
    }
    currentEventSubscriber->next = newSubscriber;
  }
}

void EventManager::unsubscribe(EventListener *eventListener, EventType eventType) {
  if (_firstEventSubscriber == nullptr || eventListener == nullptr) {
    // If we don't have a list, nothing to unsubscribe
    return;
  }
  if (_firstEventSubscriber->eventListener == eventListener && _firstEventSubscriber->eventType == eventType) {
    // If it's the first in the list, make the next one the first one, then
    // delete it
    EventSubscriber *deleteSubscriber = _firstEventSubscriber;
    _firstEventSubscriber = _firstEventSubscriber->next;
    delete deleteSubscriber;
    return;
  }
  // Otherwise, traverse the list to find it, and shuffle the list accordingly
  EventSubscriber *currentSubscriber = _firstEventSubscriber;
  EventSubscriber *previousSubscriber = nullptr;
  while (currentSubscriber != nullptr) {
    if (currentSubscriber->eventListener == eventListener && currentSubscriber->eventType == eventType) {
      // Bypass the subscriber we're deleting
      previousSubscriber->next = currentSubscriber->next;
      delete currentSubscriber;
      return;
    }
    previousSubscriber = currentSubscriber;
    currentSubscriber = currentSubscriber->next;
  }
}

bool EventManager::isSubscribed(EventListener *eventListener, EventType eventType) {
  if (_firstEventSubscriber == nullptr || eventListener == nullptr) {
    // If we don't have a list or the provided listener is a nullptr, return
    // false
    return false;
  }
  for (EventSubscriber *eventSubscriber = _firstEventSubscriber; eventSubscriber;
       eventSubscriber = eventSubscriber->next) {
    if (eventSubscriber->eventListener == eventListener && eventSubscriber->eventType == eventType) {
      // We have a match, return true
      return true;
    }
  }
  // We haven't found it, return false
  return false;
}

void EventManager::publish(EventType eventType, EventData eventData) {
  Event event(eventType, eventData);
  for (EventSubscriber *eventSubscriber = _firstEventSubscriber; eventSubscriber;
       eventSubscriber = eventSubscriber->next) {
    if (eventSubscriber->eventType == event.eventType) {
      LOG(LogLevel::LOG_DEBUG, "EventManager::publish(): EventType::", (int)event.eventType);
      eventSubscriber->eventListener->onEvent(event);
    }
  }
}

EventManager::~EventManager() {
  if (_firstEventSubscriber != nullptr) {
    EventSubscriber *subscriber = _firstEventSubscriber;
    EventSubscriber *nextSubscriber = nullptr;
    while (subscriber) {
      nextSubscriber = subscriber->next;
      delete subscriber;
      subscriber = nextSubscriber;
    }
    _firstEventSubscriber = nullptr;
  }
}
