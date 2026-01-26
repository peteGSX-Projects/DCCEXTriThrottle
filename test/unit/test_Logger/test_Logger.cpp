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
#include <gtest/gtest.h>

using namespace testing;

class LoggerTests : public Test {
protected:
  Stream stream;

  // Optional setup method
  void SetUp() override {
    // Initialise any test resources
    Logger::reset();
    Logger::setOutput(&stream);
    stream.clear();
  }

  // Optional teardown method
  void TearDown() override {}
};

TEST_F(LoggerTests, SetLogLevel) {
  // Check the default level is warn
  EXPECT_EQ(Logger::getLogLevel(), LogLevel::LOG_WARN);

  // Set debug and validate
  Logger::setLogLevel(LogLevel::LOG_DEBUG);
  EXPECT_EQ(Logger::getLogLevel(), LogLevel::LOG_DEBUG);
}

TEST_F(LoggerTests, LogErrorMessage) {
  // Setup a dummy error code and message
  const char *errorMessage = "Unknown error";
  // Log this as an error
  Logger::log(LogLevel::LOG_ERROR, "Encountered an error: ", errorMessage);

  // Construct expected output
  std::string expectedOutput = "[ERR] Encountered an error: Unknown error\r\n";

  // Check the buffer to see if it contains this
  EXPECT_EQ(stream.buffer, expectedOutput);

  // Verify all expectations were made
  testing::Mock::VerifyAndClearExpectations(&stream);
}

TEST_F(LoggerTests, LogLevels) {
  // Setup some dummy messages
  const char *errorMessage = "This is an error";
  const char *debugMessage = "This is a debug message";

  // Expect no output for a debug message with default warning, then output at
  // debug level
  Logger::log(LogLevel::LOG_DEBUG, debugMessage);
  EXPECT_EQ(stream.buffer, "");
  stream.clear();
  Logger::setLogLevel(LogLevel::LOG_DEBUG);
  Logger::log(LogLevel::LOG_DEBUG, debugMessage);
  EXPECT_EQ(stream.buffer, "[DBG] This is a debug message\r\n");
  stream.clear();

  // Should not get the debug message at info, but we should get error
  Logger::setLogLevel(LogLevel::LOG_INFO);
  Logger::log(LogLevel::LOG_DEBUG, debugMessage);
  EXPECT_EQ(stream.buffer, "");
  stream.clear();
  Logger::log(LogLevel::LOG_ERROR, errorMessage);
  EXPECT_EQ(stream.buffer, "[ERR] This is an error\r\n");
  stream.clear();

  // We should not get an error message with log level none
  Logger::setLogLevel(LogLevel::LOG_NONE);
  Logger::log(LogLevel::LOG_ERROR, errorMessage);
  EXPECT_EQ(stream.buffer, "");
  stream.clear();

  // However, we should always get a message even at none
  Logger::log(LogLevel::LOG_MESSAGE, debugMessage);
  EXPECT_EQ(stream.buffer, "[MSG] This is a debug message\r\n");

  // Verify all expectations were made
  testing::Mock::VerifyAndClearExpectations(&stream);
}

TEST_F(LoggerTests, TestMacro) {
  // Setup a dummy error code and message
  const char *errorMessage = "Unknown error";

  EXPECT_EQ(Logger::getLogLevel(), LogLevel::LOG_WARN);

  // Log this as an error
  LOG(LogLevel::LOG_ERROR, "Encountered an error: ", errorMessage);

  // Construct expected output
  std::string expectedOutput = "[ERR] Encountered an error: Unknown error\r\n";

  // Check the buffer to see if it contains this
  EXPECT_EQ(stream.buffer, expectedOutput);
  stream.clear();

  // Now log as info, which shouldn't log
  LOG(LogLevel::LOG_INFO, "Encountered an error: ", errorMessage);
  EXPECT_EQ(stream.buffer, "");

  // Verify all expectations were made
  testing::Mock::VerifyAndClearExpectations(&stream);
}
