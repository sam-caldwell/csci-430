// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include "basic_compiler/Compiler.h"
#include "clang_path.h"
#include "run_command.h"
#include "../helper/tool_exists.h"
#include "../helper/split_lines.h"

using namespace gwbasic;
using namespace e2e_helpers;

/*
 * Test Suite: E2E Mixed-Type Array Prints
 * Purpose: Confirm truncation for integer stores and float formatting.
 */
/*
Test: E2E.MixedArrayPrints_Truncation_And_FloatFormat
Inputs: BASIC program compiled and executed
Code under test: end-to-end pipeline
Expected behavior: integer arrays truncate; single/double print with 6 decimals.
*/
TEST(E2E, MixedArrayPrints_Truncation_And_FloatFormat) {
    if (!toolExists(CLANG_PATH)) GTEST_SKIP();
    const char* src =
        "10 DIM A%(3)\n"
        "11 DIM B&(3)\n"
        "12 DIM C!(3)\n"
        "13 DIM D#(3)\n"
        "20 A%(1)=1.9\n"
        "30 B&(1)=-2.2\n"
        "40 C!(1)=3.25\n"
        "50 D#(1)=4.5\n"
        "60 PRINT A%(1)\n"
        "70 PRINT B&(1)\n"
        "80 PRINT C!(1)\n"
        "90 PRINT D#(1)\n"
        "100 END\n";
    std::string ir = Compiler::compileString(src);
    std::filesystem::path tmp = std::filesystem::path("..")/"tmp"/"gwbasic_e2e_mixed_prints";
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
    ASSERT_EQ(lines.size(), 4u);
    EXPECT_EQ(lines[0], std::string("1"));        // truncated from 1.9
    EXPECT_EQ(lines[1], std::string("-2"));       // truncated from -2.2
    EXPECT_EQ(lines[2], std::string("3.250000")); // float format
    EXPECT_EQ(lines[3], std::string("4.500000")); // double format
}

