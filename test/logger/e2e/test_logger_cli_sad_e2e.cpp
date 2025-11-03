// File: test/logger/e2e/test_logger_cli_sad_e2e.cpp
// Purpose: Run the CLI with invalid log paths (directories) and ensure it still succeeds.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include "../../basic_compiler/e2e/run_command.h"

using namespace e2e_helpers;

TEST(LoggerE2E, CLI_SadPath_InvalidLogTargetsStillRuns) {
  namespace fs = std::filesystem;
  fs::path buildRoot = fs::current_path() / "build";
  fs::path bin = buildRoot / "basic_compiler" / "basic_compiler";
  ASSERT_TRUE(fs::exists(bin));

  fs::path tmpdir = fs::path("..") / "tmp" / "logger_cli_e2e_sad";
  fs::create_directories(tmpdir);
  fs::path src = tmpdir / "prog.bas";
  // Directories used as log targets (invalid as files)
  fs::path lexDir = tmpdir / "lexdir";
  fs::path synDir = tmpdir / "syndir";
  fs::path semDir = tmpdir / "semdir";
  fs::path cgdDir = tmpdir / "cgdir";
  fs::create_directories(lexDir);
  fs::create_directories(synDir);
  fs::create_directories(semDir);
  fs::create_directories(cgdDir);

  { std::ofstream f(src); f << "10 PRINT 1\n20 END\n"; }

  std::ostringstream cmd;
  cmd << '"' << bin.string() << '"'
      << " \"" << src.string() << "\""
      << " --lex-log \"" << lexDir.string() << "\""
      << " --syntax-log \"" << synDir.string() << "\""
      << " --semantic-log \"" << semDir.string() << "\""
      << " --log \"" << cgdDir.string() << "\"";
  std::string out = runCommand(cmd.str());
  // Should still produce IR to stdout
  ASSERT_NE(out.find("define i32 @main()"), std::string::npos);
}
