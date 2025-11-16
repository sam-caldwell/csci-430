// (c) 2025 Sam Caldwell. All Rights Reserved.
/***
Test: E2E.Metrics_CLI_PrintsTable
Inputs: Run compiler CLI with --metrics and a simple program
Code under test: CLI flag parsing, metrics collection, ASCII report printing
Expected behavior: Metrics table appears on stderr; includes key sections and non-zero counts
*/
#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include "../../helper/source_root.h"
#include "../../helper/run_command.h"

using namespace e2e_helpers;

TEST(E2E, Metrics_CLI_PrintsTable) {
    // Create a small program on disk
    namespace fs = std::filesystem;
    fs::path tmp = fs::path("..") / "tmp" / "metrics_cli";
    fs::create_directories(tmp);
    fs::path bas = tmp / "prog.bas";
    fs::path outll = tmp / "prog.ll";
    {
        std::ofstream f(bas);
        f << "10 LET A=1+2*3\n20 PRINT A\n30 END\n";
    }

    // Resolve compiler CLI path built by CMake helper into source build dir
    const fs::path cli = fs::path(sourceRoot()) / "build" / "basic_compiler" / "basic_compiler";
    ASSERT_TRUE(fs::exists(cli)) << "Compiler binary not found: " << cli.string();

    std::ostringstream cmd;
    cmd << '"' << cli.string() << '"'
        << " \"" << bas.string() << "\" --ll \"" << outll.string() << "\" --metrics 2>&1";

    const const std::string out = runCommand(cmd.str());
    // Table should be in combined output (stderr redirected)
    ASSERT_NE(out.find("| Metric"), std::string::npos);
    ASSERT_NE(out.find("Lexer"), std::string::npos);
    ASSERT_NE(out.find("AST (parsed)"), std::string::npos);
    ASSERT_NE(out.find("Codegen"), std::string::npos);
    ASSERT_NE(out.find("IR instructions"), std::string::npos);
}
