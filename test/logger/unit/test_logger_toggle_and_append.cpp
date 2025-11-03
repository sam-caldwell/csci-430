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
  return std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
}

TEST(Logger, ToggleEnabledState) {
  Logger log;
  fs::path outdir = fs::current_path() / "logger_tests";
  fs::create_directories(outdir);
  fs::path file = outdir / "toggle.log";

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

TEST(Logger, AppendMode) {
  Logger log;
  fs::path outdir = fs::current_path() / "logger_tests";
  fs::create_directories(outdir);
  fs::path file = outdir / "append.log";

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

