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

#ifndef EVENTSTRUCTURE_H
#define EVENTSTRUCTURE_H

#include "AppStates.h"
#include <Arduino.h>
#include <DCCEXProtocol.h>

/**
 * @brief Enum containing all the event types that subscribers can listen for, and publishers can publish Listeners and
 * publishers must use one of these when creating or listening for a valid Event
 * @details IT IS CRITICAL that the order of these enums must match the AppOrchestrator event handler function pointer
 * array in AppOrchestrator::onEvent().
 */
enum EventType {
  CommandStationConnected, // 0
  ToggleTurnout,           // 1
  LocoSelected,            // 2
  ReceivedLocoUpdate,      // 3
  ReceivedTrackPower,      // 4
  ReceivedReadLoco,        // 5
  ToggleTrackPower,        // 6
  ReceivedLocoBroadcast,   // 7
  ConnectionRetry,         // 8
  ReadLocoRetry,           // 9
  ExitMenu,                // 10
  MenuRefreshRequired,     // 11
  LocoAddressEntered,      // 12
  RequestStateChange,      // 13
  StartRoute,              // 14
  StartAutomation,         // 15
  EVENT_TYPE_COUNT // Not an event, simply enables auto subscription in AppOrchestrator::begin() and bounds checking,
                   // MUST BE LAST
};

/// @brief Structure for data relating to a Loco broadcast that can be contained
/// within EventData.
/// address - DCC adress of the Loco the braodcast is for
/// speed - Speed of the loco as interpreted from the speed byte
/// direction - Direction of the loco as interpreted from the speed byte
/// functionMap - Current state of the functions for the Loco
struct LocoBroadcast {
  int address;
  int speed;
  Direction direction;
  int functionMap;
};

/**
 * @brief Structure for receiving SelectLoco event data that contains the Loco and throttle index
 */
struct SelectLoco {
  Loco *loco;
  int throttleIndex;
};

/**
 * @brief Structure for receiving LocoAddressEntered event data that contains the loco DCC address and throttle index
 */
struct LocoAddress {
  int address;
  int throttleIndex;
};

/**
 * @brief Structure for receiving a RequestStateChange event
 */
struct StateRequest {
  AppState state;   // AppOrchestrator AppState requested
  int contextIndex; // eg. Throttle index
};

/// @brief Structure to enable supporting EventData that has various different
/// types ByteData - caters for 8 bit unsigned integer data (uint8_t x)
/// IntegerData - caters for signed integer data (int y)
/// LocoData - caters for a pointer to a DCCEXProtocol Loco object (Loco *loco)
/// NoneData - caters for events that contain no data and are notification only
/// TrackPowerData - caters for events containing track power updates
/// LocoBroadcaseData - caters for events containing Loco broadcasts
/// When adding new data types:
/// - Add type to the DataType enum
/// - Add the type to the union
/// - Add a new constructor for EventData
struct EventData {
  enum class DataType {
    ByteData,
    IntegerData,
    LocoData,
    NoneData,
    TrackPowerData,
    LocoBroadcastData,
    SelectLocoData,
    LocoAddressData,
    StateRequestData
  };
  DataType dataType;

  union {
    uint8_t byteValue;
    int intValue;
    Loco *locoValue;
    TrackPower trackPowerValue;
    LocoBroadcast locoBroadcastValue;
    SelectLoco selectLocoValue;
    LocoAddress locoAddressValue;
    StateRequest stateRequestValue;
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

  /// @brief Constructor for events containing loco broadcast information
  /// @param value LocoBroadcast data
  EventData(LocoBroadcast value) : dataType(DataType::LocoBroadcastData), locoBroadcastValue(value) {}

  /**
   * @brief Construct a new Event Data object for a SelectLoco event
   * @param loco Pointer to the loco selected
   * @param throttleIndex Index of the throttle that will control this Loco
   */
  EventData(Loco *loco, int throttleIndex) : dataType(DataType::SelectLocoData), selectLocoValue{loco, throttleIndex} {}

  /**
   * @brief Construct a new Event Data object for a LocoAddressEntered event
   * @param address DCC address entered by the user
   * @param throttleIndex Index of the throttle that will control this loco
   */
  EventData(int address, int throttleIndex)
      : dataType(DataType::LocoAddressData), locoAddressValue{address, throttleIndex} {}

  /**
   * @brief Construct a new Event Data object for a RequestStateChange event
   * @param state AppState being requested
   * @param contextIndex Contextual index to provide, eg. throttle index
   */
  EventData(AppState state, int contextIndex)
      : dataType(DataType::StateRequestData), stateRequestValue{state, contextIndex} {}
};

/// @brief Structure for each Event that is published
struct Event {
  EventType eventType = EventType::EVENT_TYPE_COUNT;
  EventData eventData;

  /// @brief Constructor for each event
  /// @param eventType A valid EventType
  /// @param eventData Valid EventData
  Event(EventType eventType, EventData eventData) : eventType(eventType), eventData(eventData) {}
};

#endif // EVENTSTRUCTURE_H
