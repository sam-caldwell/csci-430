// File: test/logger/unit/test_logger_basic.cpp
// Purpose: Verify basic logging behavior and null-sink operation.

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

TEST(Logger, DisabledDoesNotWrite) {
  Logger log;
  // Ensure a test-local directory under current working dir (which is inside build)
  fs::path outdir = fs::current_path() / "logger_tests";
  fs::create_directories(outdir);
  fs::path file = outdir / "disabled.log";

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

TEST(Logger, EnabledWritesContent) {
  Logger log;
  fs::path outdir = fs::current_path() / "logger_tests";
  fs::create_directories(outdir);
  fs::path file = outdir / "enabled.log";

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

