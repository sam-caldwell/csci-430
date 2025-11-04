// File: test/logger/unit/test_logger_toggle_and_append.cpp
// Purpose: Verify toggling enabled state and append behavior.

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
 * Test: Logger.ToggleEnabledState
 * Purpose: Ensure toggling enabled state discards/accepts writes accordingly.
 */
/*
Test: Logger.ToggleEnabledState
Inputs: Filesystem paths, log messages, toggles
Code under test: Logger component
Expected behavior: Creates directories, writes/appends as expected, handles errors
*/
TEST(Logger, ToggleEnabledState) {
  Logger log;
  const fs::path outdir = fs::current_path() / "logger_tests";
  fs::create_directories(outdir);
  const fs::path file = outdir / "toggle.log";

  ASSERT_TRUE(log.open(file.string()));
  // Initially disabled
  log() << "discard this" << '\n';

  log.setEnabled(true);
  log() << "keep this" << '\n';

  log.setEnabled(false);
  log() << "discard this too" << '\n';

  log.close();

  ASSERT_TRUE(fs::exists(file));
  std::string contents = read_all(file);
  EXPECT_EQ(contents, std::string("keep this\n"));

  fs::remove(file);
}
