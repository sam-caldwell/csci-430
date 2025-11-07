// File: test/logger/unit/test_logger_open_failure.cpp
// Purpose: Verify open() returns false for invalid path and stream sinks.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>

#include "logger/Logger.h"

namespace fs = std::filesystem;

/***
 * Test: Logger.OpenFailureReturnsFalse
 * Inputs: Filesystem paths, log messages, toggles
 * Code under test: Logger component
 * Expected behavior: Creates directories, writes/appends as expected, handles errors
 */
TEST(Logger, OpenFailureReturnsFalse) {
  logger::Logger log;
  // Attempt to open a directory path as a file (should fail)
  const fs::path dir = fs::current_path();
  bool ok = log.open(dir.string());
  EXPECT_FALSE(ok);

  // Even if enabled, stream should sink when not open
  log.setEnabled(true);
  log() << "this should be discarded" << '\n';
}

