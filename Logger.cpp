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

#include "Logger.h"

Stream *Logger::_outputStream = nullptr;
LogLevel Logger::_currentLevel = LogLevel::LOG_WARN;

void Logger::setOutput(Stream *stream) { _outputStream = stream; }

void Logger::setLogLevel(LogLevel logLevel) { _currentLevel = logLevel; }

LogLevel Logger::getLogLevel() { return _currentLevel; }

void Logger::log(LogLevel logLevel, const char *message) {
  if (_outputStream == nullptr || (logLevel > _currentLevel && logLevel != LogLevel::LOG_MESSAGE))
    return;

  _printPrefix(logLevel, _outputStream);
  _outputStream->println(message);
}

void Logger::reset() {
  _outputStream = nullptr;
  _currentLevel = LogLevel::LOG_WARN;
}

void Logger::_printPrefix(LogLevel logLevel, Stream *outputStream) {
  // Static lookup table for prefixes to save Flash
  static const char *const prefixes[] = {"[MSG] ", "", "[ERR] ", "[WRN] ", "[INF] ", "[DBG] "};
  // Assign the prefix pointer from the logLevel
  const char *prefix = (logLevel >= 0 && logLevel < 6) ? prefixes[logLevel] : "";
  outputStream->print(prefix);
}
