// File: test/logger/e2e/test_logger_cli_happy_e2e.cpp
// Purpose: Run the CLI with log flags and verify log files are produced.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include "../../basic_compiler/e2e/run_command.h"

using namespace e2e_helpers;

/***
 * Test: LoggerE2E.CLI_HappyPath_ProducesLogs
 * Purpose: Run CLI with log flags and verify log files are created with content.
 */
TEST(LoggerE2E, CLI_HappyPath_ProducesLogs) {
  namespace fs = std::filesystem;
  fs::path buildRoot = fs::current_path() / "build";
  fs::path bin = buildRoot / "basic_compiler" / "basic_compiler";
  ASSERT_TRUE(fs::exists(bin));

  fs::path tmpdir = fs::path("..") / "tmp" / "logger_cli_e2e_happy";
  fs::create_directories(tmpdir);
  fs::path src = tmpdir / "prog.bas";
  fs::path lex = tmpdir / "lex.log";
  fs::path syn = tmpdir / "syntax.log";
  fs::path sem = tmpdir / "semantic.log";
  fs::path cg  = tmpdir / "codegen.log";

  { std::ofstream f(src); f << "10 PRINT \"X\"\n20 END\n"; }

  std::ostringstream cmd;
  cmd << '"' << bin.string() << '"'
      << " \"" << src.string() << "\""
      << " --lex-log \"" << lex.string() << "\""
      << " --syntax-log \"" << syn.string() << "\""
      << " --semantic-log \"" << sem.string() << "\""
      << " --log \"" << cg.string() << "\"";
  std::string out = runCommand(cmd.str());
  ASSERT_NE(out.find("define i32 @main()"), std::string::npos);

  auto slurp = [](const fs::path& p) {
    std::ifstream in(p); return std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
  };
  ASSERT_TRUE(fs::exists(lex));
  ASSERT_TRUE(fs::exists(syn));
  ASSERT_TRUE(fs::exists(sem));
  ASSERT_TRUE(fs::exists(cg));
  EXPECT_NE(slurp(lex).find("token"), std::string::npos);
  EXPECT_NE(slurp(syn).find("line"), std::string::npos);
  EXPECT_NE(slurp(sem).find("Line"), std::string::npos);
  EXPECT_NE(slurp(cg).find("entry ->"), std::string::npos);
}
