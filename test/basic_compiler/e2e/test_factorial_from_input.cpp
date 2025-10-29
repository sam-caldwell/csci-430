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
#include "run_with_input.h"

using namespace gwbasic;
using namespace e2e_helpers;
/*
 * Test Suite: E2E Factorial Input
 * Purpose: Verify INPUT, FOR loop, and multiplication work end-to-end by
 *          compiling and running a factorial program with stdin input.
 * Components Under Test: Full compiler pipeline; scanf-based input; clang.
 * Expected Behavior: Program prints the correct factorial value for input.
 */
TEST(E2E, FactorialFromInput) {
    if (!toolExists(CLANG_PATH)) {
        GTEST_SKIP() << "clang not found (CLANG_PATH='" << CLANG_PATH << "'), skipping E2E.";
    }
    std::string src = R"(10 INPUT N
20 LET F = 1
30 FOR I = 1 TO N : LET F = F * I : NEXT I
40 PRINT F
50 END
)";
    std::string ir = Compiler::compileString(src);
    std::filesystem::path tmp = std::filesystem::temp_directory_path() / "gwbasic_e2e_fact";
    std::filesystem::create_directories(tmp);
    std::filesystem::path ll = tmp / "program.ll";
    std::filesystem::path bin = tmp / "program.out";
    { std::ofstream f(ll); f << ir; }
    std::ostringstream c5; c5 << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\""; std::string cmd = c5.str();
    int ec = std::system(cmd.c_str());
    ASSERT_EQ(ec, 0);
    std::string out = runCommandWithInput(bin.string(), "5\\n");
    ASSERT_NE(out.find("120.000000\n"), std::string::npos);
}
