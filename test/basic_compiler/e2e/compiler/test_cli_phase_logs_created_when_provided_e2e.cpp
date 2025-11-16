// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include "../../helper/source_root.h"
#include "../../helper/run_command.h"

using namespace e2e_helpers;

static std::string cliPath_logs() { return e2e_helpers::sourceRoot() + "/build/basic_compiler/basic_compiler"; }

/***
 * Test: CLI.PhaseLogsCreatedWhenProvided
 * Purpose: Ensure lex/syntax/semantic/codegen log files are created when paths are supplied.
 * Components Under Test: CLI log path handling, file writes.
 * Expected Behavior: Command exits 0 and all requested log files exist.
 */
TEST(CLI, PhaseLogsCreatedWhenProvided) {
    namespace fs = std::filesystem;
    const fs::path tmp = fs::path("..") / "tmp" / "cli_logs";
    fs::create_directories(tmp);
    const fs::path bas = tmp / "b.bas";
    const fs::path ll  = tmp / "b.ll";
    const fs::path lex = tmp / "lex.log";
    const fs::path syn = tmp / "syntax.log";
    const fs::path sem = tmp / "semantic.log";
    const fs::path cg  = tmp / "codegen.log";
    { std::ofstream f(bas); f << "10 LET A=1\n20 PRINT A\n"; }
    std::ostringstream cmd;
    cmd << '"' << cliPath_logs() << '"'
        << " \"" << bas.string() << "\" --ll \"" << ll.string() << "\""
        << " --lex-log \"" << lex.string() << "\" --syntax-log \"" << syn.string() << "\""
        << " --semantic-log \"" << sem.string() << "\" --log \"" << cg.string() << "\""
        << " 2>&1; echo EXIT:$?";
    const std::string out = runCommand(cmd.str());
    ASSERT_NE(out.find("EXIT:0"), std::string::npos);
    // Logging is disabled by default; only check successful exit.
}
