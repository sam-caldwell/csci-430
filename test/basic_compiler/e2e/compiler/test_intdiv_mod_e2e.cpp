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
 * Test: E2E.IntDiv_Mod_Behavior
 * Inputs: Program printing results of integer division (\\) and MOD with sign cases and decimals
 * Code under test: End-to-end pipeline
 * Expected behavior: \\\ uses truncation toward zero; MOD has sign of dividend and truncates operands
 */
TEST(E2E, IntDiv_Mod_Behavior) {
    if (!toolExists(CLANG_PATH)) GTEST_SKIP();
    const char* src =
        "10 PRINT 5 \\ 2\n"       // 2
        "20 PRINT -5 \\ 2\n"      // -2
        "30 PRINT 5 \\ -2\n"      // -2
        "40 PRINT -5 \\ -2\n"     // 2
        "50 PRINT 5 MOD 2\n"      // 1
        "60 PRINT -5 MOD 2\n"     // -1
        "70 PRINT 5 MOD -2\n"     // 1
        "80 PRINT 5.9 \\ 2.1\n"   // 2 (truncate operands)
        "90 PRINT 5.9 MOD 2.1\n"  // 1 (truncate operands)
        "100 END\n";

    const std::string ir = Compiler::compileString(src);
    const std::filesystem::path tmp = std::filesystem::path("..")/"tmp"/"gwbasic_e2e_intdiv_mod";
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
    ASSERT_EQ(lines.size(), 9u);
    EXPECT_EQ(lines[0], std::string("2"));
    EXPECT_EQ(lines[1], std::string("-2"));
    EXPECT_EQ(lines[2], std::string("-2"));
    EXPECT_EQ(lines[3], std::string("2"));
    EXPECT_EQ(lines[4], std::string("1"));
    EXPECT_EQ(lines[5], std::string("-1"));
    EXPECT_EQ(lines[6], std::string("1"));
    EXPECT_EQ(lines[7], std::string("2"));
    EXPECT_EQ(lines[8], std::string("1"));
}
