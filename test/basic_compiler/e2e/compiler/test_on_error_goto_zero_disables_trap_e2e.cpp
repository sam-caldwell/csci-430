// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include "basic_compiler/Compiler.h"
#include "clang_path.h"
#include "../../helper/run_command.h"
#include "../../helper/tool_exists.h"

using namespace gwbasic;
using namespace e2e_helpers;

/***
 * Test: E2E.OnErrorGotoZeroDisablesTrap
 * Purpose: Ensure ON ERROR GOTO 0 disables the error handler so ERROR exits program flow.
 * Components Under Test: Semantics/codegen for disabling trap, End-to-End run.
 * Expected Behavior: Only the value before ERROR is printed; subsequent prints do not occur.
 */
TEST(E2E, OnErrorGotoZeroDisablesTrap) {
    if (!toolExists(CLANG_PATH)) {
        GTEST_SKIP() << "clang not found (CLANG_PATH='" << CLANG_PATH << "'), skipping E2E.";
    }
    std::string src = R"(10 ON ERROR GOTO 0
20 PRINT 1
30 ERROR 5
40 PRINT 2
)";
    std::string ir = Compiler::compileString(src);
    std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_on_error_zero";
    std::filesystem::create_directories(tmp);
    auto ll = tmp / "program.ll";
    auto bin = tmp / "program.out";
    { std::ofstream f(ll); f << ir; }
    std::ostringstream cmd; cmd << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    cmd << " -lm";
#endif
    ASSERT_EQ(std::system(cmd.str().c_str()), 0);
    std::ostringstream run; run << '"' << bin.string() << '"';
    std::string out = runCommand(run.str());
    // With trap disabled, program branches to exit at ERROR; expect only '1' printed
    ASSERT_NE(out.find("1\n"), std::string::npos);
    ASSERT_EQ(out.find("2\n"), std::string::npos);
}
