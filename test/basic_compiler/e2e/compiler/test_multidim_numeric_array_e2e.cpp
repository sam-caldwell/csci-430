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
 * Test: E2E.MultiDim_Numeric_Array_Assign_And_Print
 * Purpose: Verify multi-dimensional numeric arrays assign/print correctly end-to-end.
 * Components Under Test: Parser/Codegen for DIM A(,)/element assign/PRINT; End-to-End run.
 * Expected Behavior: Output shows the assigned number.
 */
TEST(E2E, MultiDim_Numeric_Array_Assign_And_Print) {
    if (!toolExists(CLANG_PATH)) GTEST_SKIP();
    const char* src =
        "10 DIM A(2,3)\n"
        "20 A(1,2)=8\n"
        "30 PRINT A(1,2)\n"
        "40 END\n";
    const std::string ir = Compiler::compileString(src);
    const std::filesystem::path tmp = std::filesystem::path("..")/"tmp"/"gwbasic_e2e_num_mdim";
    std::filesystem::create_directories(tmp);
    auto ll = tmp/"program.ll"; auto bin = tmp/"program.out";
    { std::ofstream f(ll); f << ir; }
    std::ostringstream cmd; cmd << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    cmd << " -lm";
#endif
    ASSERT_EQ(std::system(cmd.str().c_str()), 0);
    const std::string out = runCommand(std::string("\"") + bin.string() + "\"");
    auto lines = splitLines(out);
    ASSERT_EQ(lines.size(), 1u);
    EXPECT_EQ(lines[0], std::string("8"));
}
