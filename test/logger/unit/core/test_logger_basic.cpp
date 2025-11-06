// File: test/logger/unit/test_logger_basic.cpp
// Purpose: Verify basic logging behavior and null-sink operation.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>

#include "logger/Logger.h"

namespace fs = std::filesystem;
using logger::Logger;

/***
 * Test: Logger.DisabledDoesNotWrite
 * Purpose: Ensure that when disabled, logger discards writes and file remains empty.
 */
/*
Test: Logger.DisabledDoesNotWrite
Inputs: Filesystem paths, log messages, toggles
Code under test: Logger component
Expected behavior: Creates directories, writes/appends as expected, handles errors
*/
TEST(Logger, DisabledDoesNotWrite) {
  Logger log;
  // Ensure a test-local directory under current working dir (which is inside build)
  const fs::path outdir = fs::current_path() / "logger_tests";
  fs::create_directories(outdir);
  const fs::path file = outdir / "disabled.log";

  ASSERT_TRUE(log.open(file.string(), /*append=*/false));
  // Default is disabled; writes go to null sink
  log() << "hello" << '\n' << 123 << '\n';
  log.close();

  ASSERT_TRUE(fs::exists(file));
  // File should be empty because all writes were discarded
  std::ifstream in(file);
  std::string contents((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
  EXPECT_TRUE(contents.empty());

  // Cleanup
  fs::remove(file);
}

