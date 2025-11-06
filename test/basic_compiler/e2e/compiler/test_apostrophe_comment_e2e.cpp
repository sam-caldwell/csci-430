// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include "basic_compiler/Compiler.h"
#include "clang_path.h"
#include "run_command.h"
#include "../../helper/tool_exists.h"
#include "../../helper/split_lines.h"

using namespace gwbasic;
using namespace e2e_helpers;

/***
 * Test: E2E.ApostropheComment_IgnoresRestOfLine
 * Purpose: Validate runtime behavior: apostrophe comments are skipped.
 * Expected: Two lines with numbers 1 and 2.
 */
/*
Test: E2E.ApostropheComment_IgnoresRestOfLine
Inputs: Program with inline apostrophe comments containing invalid tokens
Code under test: Full pipeline
Expected behavior: Runs and prints two lines: 1 and 2
*/
TEST(E2E, ApostropheComment_IgnoresRestOfLine) {
    if (!toolExists(CLANG_PATH)) { GTEST_SKIP() << "clang not found"; }
    const char* src =
        "10 PRINT 1 ' !! $$$ invalid tokens after comment\n"
        "20 PRINT 2 ' more !@# junk\n"
        "30 END\n";
    std::string ir = Compiler::compileString(src);
    std::filesystem::path tmp = std::filesystem::path("..")/"tmp"/"gwbasic_e2e_apos_comment";
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
    EXPECT_EQ(lines[0], std::string("1"));
    EXPECT_EQ(lines[1], std::string("2"));
}
