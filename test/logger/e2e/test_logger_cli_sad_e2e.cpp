// File: test/logger/e2e/test_logger_cli_sad_e2e.cpp
// Purpose: Run the CLI with invalid log paths (directories) and ensure it still succeeds.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include "../../basic_compiler/e2e/run_command.h"

using namespace e2e_helpers;

/***
 * Test: LoggerE2E.CLI_SadPath_InvalidLogTargetsStillRuns
 * Purpose: Verify CLI runs and emits IR even when log paths are directories.
 */
TEST(LoggerE2E, CLI_SadPath_InvalidLogTargetsStillRuns) {
  namespace fs = std::filesystem;
  const fs::path buildRoot = fs::current_path() / "build";
  const fs::path bin = buildRoot / "basic_compiler" / "basic_compiler";
  ASSERT_TRUE(fs::exists(bin));

  const fs::path tmpdir = fs::path("..") / "tmp" / "logger_cli_e2e_sad";
  fs::create_directories(tmpdir);
  const fs::path src = tmpdir / "prog.bas";
  // Directories used as log targets (invalid as files)
  const fs::path lexDir = tmpdir / "lexdir";
  const fs::path synDir = tmpdir / "syndir";
  const fs::path semDir = tmpdir / "semdir";
  const fs::path cgdDir = tmpdir / "cgdir";
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
