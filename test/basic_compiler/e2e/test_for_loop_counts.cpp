// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include "basic_compiler/Compiler.h"
#include "clang_path.h"
#include "run_command.h"
#include "tool_exists.h"

using namespace gwbasic;
using namespace e2e_helpers;
/*
 * Test Suite: E2E For Loop Counts
 * Purpose: Validate FOR/NEXT iteration behavior end-to-end by compiling
 *          and executing a program that prints 1..N.
 * Components Under Test: Full compiler pipeline; FOR lowering; clang.
 * Expected Behavior: Program prints numbers 1 through 5, each on its own line.
 */
TEST(E2E, ForLoopCounts) {
    if (!toolExists(CLANG_PATH)) {
        GTEST_SKIP() << "clang not found (CLANG_PATH='" << CLANG_PATH << "'), skipping E2E.";
    }
    std::string src = R"(10 LET N = 5
20 FOR I = 1 TO N : PRINT I : NEXT I
30 END
)";
    std::string ir = Compiler::compileString(src);

    std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_for";
    std::filesystem::create_directories(tmp);
    std::filesystem::path ll = tmp / "program.ll";
    std::filesystem::path bin = tmp / "program.out";
    { std::ofstream f(ll); f << ir; }

    std::ostringstream c2; c2 << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\""; std::string cmd = c2.str();
    int ec = std::system(cmd.c_str());
    ASSERT_EQ(ec, 0);
    std::ostringstream r2; r2 << '"' << bin.string() << '"'; std::string out = runCommand(r2.str());
    ASSERT_NE(out.find("1.000000\n2.000000\n3.000000\n4.000000\n5.000000\n"), std::string::npos);
}
