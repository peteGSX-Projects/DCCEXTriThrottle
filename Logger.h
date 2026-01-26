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

#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>

/// @brief Macro for shorter logging calls, assumes a pointer to a Logger instance called _logger
/// LOG(LogLevel, "Message");
#define LOG(level, ...) Logger::log(level, __VA_ARGS__)

/// @brief Define valid log levels in ascending order
enum LogLevel { LOG_MESSAGE, LOG_NONE, LOG_ERROR, LOG_WARN, LOG_INFO, LOG_DEBUG };

/// @brief Class to enable simple logging to a Stream object with different log levels
/// This enables embedding permanent error, warn, info, and debug messages in the software, with the user defining the
/// log level at compile time if more diagnostics are required
class Logger {
public:
  /**
   * @brief Set the Output object
   * @param stream Pointer to a Stream object to output to
   */
  static void setOutput(Stream *stream);

  /// @brief Set the log level
  /// @param logLevel Valid LogLevel
  static void setLogLevel(LogLevel logLevel);

  /// @brief Get the current log level
  /// @return LogLevel
  static LogLevel getLogLevel();

  /**
   * @brief Log a simple message
   * @param logLevel LogLevel the message is targeted for
   * @param message Null terminated char array
   */
  static void log(LogLevel logLevel, const char *message);

  /**
   * @brief Templated log message
   * @tparam T Value type to substitute in the template
   * @param logLevel LogLevel the message is targeted for
   * @param message Null terminated char array
   * @param value Value to append to the log message
   */
  template <typename T> static void log(LogLevel logLevel, const char *message, T value) {
    if (_outputStream == nullptr || (logLevel > _currentLevel && logLevel != LogLevel::LOG_MESSAGE))
      return;

    _printPrefix(logLevel, _outputStream);
    _outputStream->print(message);
    _outputStream->println(value);
  }

  /**
   * @brief Reset Logger to default attributes - no output, and LogLevel::LOG_WARN
   */
  static void reset();

private:
  static Stream *_outputStream;
  static LogLevel _currentLevel;

  /**
   * @brief
   * @param logLevel
   * @param outputStream
   */
  static void _printPrefix(LogLevel logLevel, Stream *outputStream);
};

#endif // LOGGER_H
