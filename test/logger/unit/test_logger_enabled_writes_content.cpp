// File: test/logger/unit/test_logger_enabled_writes_content.cpp
// Purpose: Verify that when enabled, logger writes content to file.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>

#include "logger/Logger.h"

namespace fs = std::filesystem;
using logger::Logger;

static std::string read_file(const fs::path& p) {
  std::ifstream in(p);
  std::string s, line;
  while (std::getline(in, line)) {
    s += line;
    s.push_back('\n');
  }
  return s;
}

/***
 * Test: Logger.EnabledWritesContent
 * Purpose: Ensure that enabling the logger causes content to be written to disk.
 */
/*
Test: Logger.EnabledWritesContent
Inputs: Filesystem paths, log messages, toggles
Code under test: Logger component
Expected behavior: Creates directories, writes/appends as expected, handles errors
*/
TEST(Logger, EnabledWritesContent) {
  Logger log;
  const fs::path outdir = fs::current_path() / "logger_tests";
  fs::create_directories(outdir);
  const fs::path file = outdir / "enabled.log";

  ASSERT_TRUE(log.open(file.string()));
  log.setEnabled(true);
  log() << "line one" << '\n';
  log().flush();
  log() << "line two: " << 42 << '\n';
  log.close();

  ASSERT_TRUE(fs::exists(file));
  std::string contents = read_file(file);
  EXPECT_EQ(contents, std::string("line one\nline two: 42\n"));

  // Cleanup
  fs::remove(file);
}
