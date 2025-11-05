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
/*
Test: LoggerE2E.CLI_HappyPath_ProducesLogs
Inputs: Filesystem paths, log messages, toggles
Code under test: Logger component
Expected behavior: Creates directories, writes/appends as expected, handles errors
*/
TEST(LoggerE2E, CLI_HappyPath_ProducesLogs) {
  namespace fs = std::filesystem;
  auto find_bin = []() -> std::optional<std::filesystem::path> {
    namespace fs = std::filesystem;
    const fs::path root = fs::current_path();
    const fs::path build = root / "build";
    const fs::path candidates[] = {
      build / "basic_compiler" / "basic_compiler",                     // multi-config style
      build / "cmake-build-debug" / "basic_compiler",                  // Ninja single-config (Debug)
      build / "cmake-build-release" / "basic_compiler",                // Ninja single-config (Release)
      build / "Debug" / "basic_compiler",                              // MSVC/Other
      build / "Release" / "basic_compiler",
      build / "RelWithDebInfo" / "basic_compiler"
    };
    for (const auto &p : candidates) if (fs::exists(p) && fs::is_regular_file(p)) return p;
    // Fallback: scan build/ recursively for a regular file named "basic_compiler"
    if (fs::exists(build) && fs::is_directory(build)) {
      for (auto it = fs::recursive_directory_iterator(build); it != fs::recursive_directory_iterator(); ++it) {
        if (it->path().filename() == "basic_compiler" && fs::is_regular_file(*it)) return it->path();
      }
    }
    return std::nullopt;
  };
  const auto bin = find_bin();
  if (!bin) GTEST_SKIP() << "basic_compiler CLI not built in this configuration";

  const fs::path tmpdir = fs::path("..") / "tmp" / "logger_cli_e2e_happy";
  fs::create_directories(tmpdir);
  const fs::path src = tmpdir / "prog.bas";
  const fs::path lex = tmpdir / "lex.log";
  const fs::path syn = tmpdir / "syntax.log";
  const fs::path sem = tmpdir / "semantic.log";
  const fs::path cg  = tmpdir / "codegen.log";

  { std::ofstream f(src); f << "10 PRINT \"X\"\n20 END\n"; }

  std::ostringstream cmd;
  cmd << '"' << bin->string() << '"'
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
