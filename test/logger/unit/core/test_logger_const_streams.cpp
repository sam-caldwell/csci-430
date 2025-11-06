// File: test/logger/unit/test_logger_const_streams.cpp
// Purpose: Exercise const stream() overload paths (open+enabled and sink).

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include "logger/Logger.h"

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

  std::ifstream in(file);
  std::string s((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
  EXPECT_NE(s.find("via const\n"), std::string::npos);
  EXPECT_EQ(s.find("discard\n"), std::string::npos);

  fs::remove(file);
}

