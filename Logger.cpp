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

void Logger::log(LogLevel logLevel, const char *format, ...) {
  if (_outputStream == nullptr)
    return;
  if (logLevel <= _currentLevel) {
    // Setup the prefix
    const char *prefix;
    switch (logLevel) {
    case LogLevel::LOG_MESSAGE:
      prefix = "[MSG] ";
      break;
    case LogLevel::LOG_ERROR:
      prefix = "[ERR] ";
      break;
    case LogLevel::LOG_WARN:
      prefix = "[WRN] ";
      break;
    case LogLevel::LOG_INFO:
      prefix = "[INF] ";
      break;
    case LogLevel::LOG_DEBUG:
      prefix = "[DBG] ";
      break;
    default:
      prefix = "";
      break;
    }

    // Use fixed size buffer to save Flash
    char buffer[64];

    // Copy prefix first
    strncpy(buffer, prefix, sizeof(buffer));
    size_t prefixLen = strlen(prefix);

    // Format the message directly into the remaining space
    va_list args;
    va_start(args, format);
    // Write into the buffer starting after the prefix
    vsnprintf(buffer + prefixLen, sizeof(buffer) - prefixLen, format, args);
    va_end(args);

    // Output formatted message
    _outputStream->println(buffer);
  }
}

void Logger::reset() {
  _outputStream = nullptr;
  _currentLevel = LogLevel::LOG_WARN;
}
