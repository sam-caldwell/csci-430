// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include "basic_compiler/Compiler.h"
#include "../../helper/clang_path.h"
#include "../../helper/run_command.h"
#include "../../helper/tool_exists.h"

using namespace gwbasic;
using namespace e2e_helpers;

/***
 * Test Suite: E2E For Loop (descending)
 * Purpose: Validate FOR with STEP -1 iterates downward inclusively.
 * Components Under Test: Compiler (compileString/compileFile), Clang driver, runtime output
 * Expected Behavior: Program output matches assertions in test.
 */
TEST(E2E, ForLoopDescendingCounts) {
    if (!toolExists(CLANG_PATH)) {
        GTEST_SKIP() << "clang not found (CLANG_PATH='" << CLANG_PATH << "'), skipping E2E.";
    }
    std::string src = R"(10 FOR I = 5 TO 1 STEP -1
20 PRINT I
30 NEXT I
40 END
)";
    std::string ir = Compiler::compileString(src);

    std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_for_desc";
    std::filesystem::create_directories(tmp);
    std::filesystem::path ll = tmp / "program.ll";
    std::filesystem::path bin = tmp / "program.out";
    { std::ofstream f(ll); f << ir; }

    std::ostringstream c2; c2 << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    c2 << " -lm";
#endif
    std::string cmd = c2.str();
    int ec = std::system(cmd.c_str());
    ASSERT_EQ(ec, 0);
    std::ostringstream r2; r2 << '"' << bin.string() << '"'; std::string out = runCommand(r2.str());
    ASSERT_NE(out.find("5\n4\n3\n2\n1\n"), std::string::npos);
}
