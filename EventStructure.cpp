#include "EventStructure.h"

const char *eventTypeToString(EventType eventType) {
  switch (eventType) {
  case CommandStationConnected:
    return "CommandStationConnected";
  case ReceivedRosterList:
    return "ReceivedRosterList";
  case LocoSelected:
    return "LocoSelected";
  case ReceivedLocoUpdate:
    return "ReceivedLocoUpdate";
  case ReceivedTrackPower:
    return "ReceivedTrackPower";
  case ReceivedReadLoco:
    return "ReceivedReadLoco";
  case ToggleTrackPower:
    return "ToggleTrackPower";
  case ReceivedLocoBroadcast:
    return "ReceivedLocoBroadcast";
  case ConnectionRetry:
    return "ConnectionRetry";
  case ReadLocoRetry:
    return "ReadLocoRetry";
  case ExitMenu:
    return "ExitMenu";
  case MenuRefreshRequired:
    return "MenuRefreshRequired";
  default:
    return "UNKNOWN_EVENT";
  }
}
