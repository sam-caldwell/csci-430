// File: test/logger/unit/test_logger_creates_directories.cpp
// Purpose: Verify logger creates parent directories for log path.

#include <gtest/gtest.h>
#include <filesystem>

#include "logger/Logger.h"

namespace fs = std::filesystem;

/***
 * Test: Logger.CreatesParentDirectories
 * Purpose: Opening a file path should create missing parent directories.
 */
TEST(Logger, CreatesParentDirectories) {
  logger::Logger log;
  const fs::path base = fs::current_path() / "logger_tests" / "deep" / "nest";
  const fs::path file = base / "dirs.log";

  // Ensure base does not exist beforehand; then open should create it.
  fs::remove_all(base);
  ASSERT_FALSE(fs::exists(base));

  ASSERT_TRUE(log.open(file.string()));
  log.setEnabled(true);
  log() << "ok" << '\n';
  log.close();

  ASSERT_TRUE(fs::exists(file));

  // Cleanup: remove only our subdirectory to avoid races with other tests
  fs::remove_all(base.parent_path()); // removes .../logger_tests/deep
}
