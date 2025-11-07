// File: test/logger/e2e/test_logger_cli_sad_e2e.cpp
// Purpose: Run the CLI with invalid log paths (directories) and ensure it still succeeds.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include "../../../basic_compiler/helper/run_command.h"

using namespace e2e_helpers;

/*
 * Test: LoggerE2E.CLI_SadPath_InvalidLogTargetsStillRuns
 * Inputs: Filesystem paths, log messages, toggles
 * Code under test: Logger component
 * Expected behavior: Creates directories, writes/appends as expected, handles errors
 */
TEST(LoggerE2E, CLI_SadPath_InvalidLogTargetsStillRuns) {
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
  cmd << '"' << bin->string() << '"'
      << " \"" << src.string() << "\""
      << " --lex-log \"" << lexDir.string() << "\""
      << " --syntax-log \"" << synDir.string() << "\""
      << " --semantic-log \"" << semDir.string() << "\""
      << " --log \"" << cgdDir.string() << "\"";
  std::string out = runCommand(cmd.str());
  // Should still produce IR to stdout
  ASSERT_NE(out.find("define i32 @main()"), std::string::npos);
}
