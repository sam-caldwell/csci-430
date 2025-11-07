// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include "source_root.h"
#include "run_command.h"

using namespace e2e_helpers;

static std::string cliPath2() { return e2e_helpers::sourceRoot() + "/build/basic_compiler/basic_compiler"; }

TEST(CLI, UnknownArgumentYieldsUsageExit2) {
    namespace fs = std::filesystem;
    const fs::path tmp = fs::path("..") / "tmp" / "cli_unknown";
    fs::create_directories(tmp);
    const fs::path bas = tmp / "prog.bas";
    { std::ofstream f(bas); f << "10 PRINT 42\n"; }
    const std::string cmd = '"' + cliPath2() + '"' + " \"" + bas.string() + "\" --bogus 2>&1; echo EXIT:$?";
    const std::string out = runCommand(cmd);
    ASSERT_NE(out.find("Unknown argument:"), std::string::npos);
    ASSERT_NE(out.find("Usage:"), std::string::npos);
    ASSERT_NE(out.find("EXIT:2"), std::string::npos);
}

TEST(CLI, InvalidTargetTripleExits2) {
    namespace fs = std::filesystem;
    const fs::path tmp = fs::path("..") / "tmp" / "cli_target";
    fs::create_directories(tmp);
    const fs::path bas = tmp / "prog.bas";
    { std::ofstream f(bas); f << "10 PRINT 1\n"; }
    const std::string cmd = '"' + cliPath2() + '"' + " \"" + bas.string() + "\" --target not_a_real_triple 2>&1; echo EXIT:$?";
    const std::string out = runCommand(cmd);
    ASSERT_NE(out.find("unsupported target triple"), std::string::npos);
    ASSERT_NE(out.find("EXIT:2"), std::string::npos);
}

TEST(CLI, PrintTriplePrintsAndExits0) {
    // CLI expects an input path; print-triple exits before reading it
    const std::string cmd = '"' + cliPath2() + '"' + " dummy.bas --print-triple 2>&1; echo EXIT:$?";
    const std::string out = runCommand(cmd);
    // Expect something like x86_64-apple-darwin or arm64-apple-macosx...
    ASSERT_NE(out.find("-"), std::string::npos);
    ASSERT_NE(out.find("EXIT:0"), std::string::npos);
}
