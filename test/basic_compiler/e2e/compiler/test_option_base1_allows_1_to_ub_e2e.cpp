// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include "basic_compiler/Compiler.h"
#include "../../helper/clang_path.h"
#include "../../helper/run_command.h"
#include "../../helper/tool_exists.h"
#include "../../helper/split_lines.h"

using namespace gwbasic;
using namespace e2e_helpers;

/***
 * Test: E2E.OptionBase1_Allows_1_To_Ub
 * Purpose: Validate OPTION BASE 1 makes valid indices 1..UB inclusive for arrays.
 * Components Under Test: Semantics (option base), Codegen array indexing, End-to-End run.
 * Expected Behavior: Writing/reading at indices 1 and UB succeed and print expected values.
 */
TEST(E2E, OptionBase1_Allows_1_To_Ub) {
    if (!toolExists(CLANG_PATH)) GTEST_SKIP();
    const char* src =
        "10 OPTION BASE 1\n"
        "20 DIM A(3)\n"
        "30 A(1)=5\n"
        "40 A(3)=7\n"
        "50 PRINT A(1)\n"
        "60 PRINT A(3)\n"
        "70 END\n";
    std::string ir = Compiler::compileString(src);
    std::filesystem::path tmp = std::filesystem::path("..")/"tmp"/"gwbasic_e2e_optbase1";
    std::filesystem::create_directories(tmp);
    auto ll = tmp/"program.ll"; auto bin = tmp/"program.out";
    { std::ofstream f(ll); f << ir; }
    std::ostringstream cmd; cmd << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    cmd << " -lm";
#endif
    ASSERT_EQ(std::system(cmd.str().c_str()), 0);
    std::string out = runCommand(std::string("\"") + bin.string() + "\"");
    auto lines = splitLines(out);
    ASSERT_EQ(lines.size(), 2u);
    EXPECT_EQ(lines[0], std::string("5"));
    EXPECT_EQ(lines[1], std::string("7"));
}
