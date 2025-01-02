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

#ifndef EVENTSTRUCTURE_H
#define EVENTSTRUCTURE_H

#include <Arduino.h>
#include <DCCEXProtocol.h>

/// @brief Enum containing all the event types that subscribers can listen for, and publishers can publish
/// Listeners and publishers must use one of these when creating or listening for a valid Event
enum EventType {
  CommandStationSelected,
  ReceivedRosterList,
  LocoSelected,
  ReceivedLocoUpdate,
  ReceivedTrackPower,
  ReceivedReadLoco,
  ToggleTrackPower
};

/// @brief Structure to enable supporting EventData that has various different types
/// Byte - caters for 8 bit unsigned integer data (uint8_t x)
/// Integer - caters for signed integer data (int y)
/// Loco - caters for a pointer to a DCCEXProtocol Loco object (Loco *loco)
/// None - caters for events that contain no data and are notification only
/// When adding new data types:
/// - Add type to the DataType enum
/// - Add the type to the union
/// - Add a new constructor for EventData
struct EventData {
  enum class DataType { ByteData, IntegerData, LocoData, NoneData, TrackPowerData };
  DataType dataType;

  union {
    uint8_t byteValue;
    int intValue;
    Loco *locoValue;
    TrackPower trackPowerValue;
  };

  /// @brief Constructor for events with a uint8_t parameter
  /// @param value 8 bit integer
  EventData(uint8_t value) : dataType(DataType::ByteData), byteValue(value) {}

  /// @brief Constructor for events with an int parameter
  /// @param value Signed integer
  EventData(int value) : dataType(DataType::IntegerData), intValue(value) {}

  /// @brief Constructor for events with a Loco pointer
  /// @param value Pointer to a loco object
  EventData(Loco *value) : dataType(DataType::LocoData), locoValue(value) {}

  /// @brief Constructor for events with no data or parameters
  EventData() : dataType(DataType::NoneData) {}

  /// @brief Constructor for events containing track power
  EventData(TrackPower value) : dataType(DataType::TrackPowerData), trackPowerValue(value) {}
};

/// @brief Structure for each Event that is published
struct Event {
  EventType eventType;
  EventData eventData;

  /// @brief Constructor for each event
  /// @param eventType A valid EventType
  /// @param eventData Valid EventData
  Event(EventType eventType, EventData eventData) : eventType(eventType), eventData(eventData) {}
};

#endif // EVENTSTRUCTURE_H
