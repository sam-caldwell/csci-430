// File: test/logger/unit/test_logger_const_streams.cpp
/***
 * Test: Logger.ConstStreamOverloads
 * Purpose: Exercise const stream() overload paths (enabled writes and disabled sink).
 * Components Under Test: logger::Logger const operator() stream accessor.
 * Expected Behavior: Enabled writes are persisted; disabled writes are discarded.
 */

#include "logger/Logger.h"
#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>

namespace fs = std::filesystem;
using logger::Logger;

TEST(Logger, ConstStreamOverloads) {
  Logger log;
  const fs::path outdir = fs::current_path() / "logger_tests";
  fs::create_directories(outdir);
  const fs::path file = outdir / "const_streams.log";

  ASSERT_TRUE(log.open(file.string()));
  log.setEnabled(true);

  // Use const overload (ofs_ path)
  const Logger& clog = log;
  clog() << "via const" << '\n';
  log.close();
  ASSERT_TRUE(fs::exists(file));

  // Disabled sink via const overload
  ASSERT_TRUE(log.open(file.string(), /*append=*/true));
  log.setEnabled(false);
  const Logger& clog2 = log;
  clog2() << "discard" << '\n';
  log.close();

  std::ifstream input(file);
  const std::string contents((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
  EXPECT_NE(contents.find("via const\n"), std::string::npos);
  EXPECT_EQ(contents.find("discard\n"), std::string::npos);

  fs::remove(file);
}
