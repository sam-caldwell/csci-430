// File: test/logger/unit/test_logger_enabled_writes_content.cpp
// Purpose: Verify that when enabled, logger writes content to file.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>

#include "logger/Logger.h"

namespace fs = std::filesystem;
using logger::Logger;

static std::string read_file(const fs::path& path) {
  std::ifstream input(path);
  std::string buffer;
  std::string line;
  while (std::getline(input, line)) {
    buffer += line;
    buffer.push_back('\n');
  }
  return buffer;
}

/***
 * Test: Logger.EnabledWritesContent
 * Purpose: Ensure that enabling the logger causes content to be written to disk.
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
  constexpr int kAnswer = 42;
  log() << "line two: " << kAnswer << '\n';
  log.close();

  ASSERT_TRUE(fs::exists(file));
  const std::string contents = read_file(file);
  EXPECT_EQ(contents, std::string("line one\nline two: 42\n"));

  // Cleanup
  fs::remove(file);
}
