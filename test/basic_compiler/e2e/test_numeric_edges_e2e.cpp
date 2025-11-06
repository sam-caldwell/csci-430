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
Test: E2E.NumericEdges_Int16_Long_Single
Inputs: Program assigns edge values near Int16/Long bounds and a large number to Single
Code under test: End-to-end compile/link/run
Expected behavior: Int16/Long truncate into range without overflow; Single rounds to nearest representable
*/
TEST(E2E, NumericEdges_Int16_Long_Single) {
    if (!toolExists(CLANG_PATH)) GTEST_SKIP();
    const char* src =
        "10 A% = 32767.9\n"         // stays 32767
        "20 B% = -32768.9\n"        // stays -32768
        "30 C& = 2147483647.9\n"     // stays 2147483647
        "40 D& = -2147483648.9\n"    // stays -2147483648
        "50 E! = 16777217\n"        // rounds to 16777216 in single
        "60 PRINT A%\n"
        "70 PRINT B%\n"
        "80 PRINT C&\n"
        "90 PRINT D&\n"
        "100 PRINT E!\n"
        "110 END\n";
    std::string ir = Compiler::compileString(src);
    std::filesystem::path tmp = std::filesystem::path("..")/"tmp"/"gwbasic_e2e_numeric_edges";
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
    ASSERT_EQ(lines.size(), 5u);
    EXPECT_EQ(lines[0], std::string("32767"));
    EXPECT_EQ(lines[1], std::string("-32768"));
    EXPECT_EQ(lines[2], std::string("2147483647"));
    EXPECT_EQ(lines[3], std::string("-2147483648"));
    EXPECT_EQ(lines[4], std::string("16777216.000000"));
}

