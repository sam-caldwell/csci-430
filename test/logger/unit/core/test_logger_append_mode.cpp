// File: test/logger/unit/test_logger_append_mode.cpp
// Purpose: Verify append mode preserves existing content and appends new lines.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>

#include "logger/Logger.h"

namespace fs = std::filesystem;
using logger::Logger;

static std::string read_all(const fs::path& p) {
  std::ifstream in(p);
  return {(std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>()};
}

/***
 * Test: Logger.AppendMode
 * Purpose: Ensure reopening a file in append mode preserves existing content.
 */
/*
Test: Logger.AppendMode
Inputs: Filesystem paths, log messages, toggles
Code under test: Logger component
Expected behavior: Creates directories, writes/appends as expected, handles errors
*/
TEST(Logger, AppendMode) {
  Logger log;
  const fs::path outdir = fs::current_path() / "logger_tests";
  fs::create_directories(outdir);
  const fs::path file = outdir / "append.log";

  ASSERT_TRUE(log.open(file.string()));
  log.setEnabled(true);
  log() << "first" << '\n';
  log.close();

  // Reopen in append and write more
  ASSERT_TRUE(log.open(file.string(), /*append=*/true));
  log.setEnabled(true);
  log() << "second" << '\n';
  log.close();

  ASSERT_TRUE(fs::exists(file));
  std::string contents = read_all(file);
  EXPECT_EQ(contents, std::string("first\nsecond\n"));

  fs::remove(file);
}

