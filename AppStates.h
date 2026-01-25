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

#ifndef APPSTATES_H
#define APPSTATES_H

/**
 * @brief Enumeration of valid state machine states
 * @details Remember to add to AppOrchestrator::_appStateToString() when adding new states
 */
enum class AppState {
  Startup,
  Throttle,
  ConnectionError,
  Menu,
  UserEntry,
  EnterLocoAddress,
  APP_STATE_COUNT // Not a state, used for validation checks
};

#endif // APPSTATES_H
