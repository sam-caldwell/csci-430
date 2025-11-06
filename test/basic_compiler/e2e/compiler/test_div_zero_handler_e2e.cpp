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

/*
Test: E2E.DivisionByZero_TriggersErrorHandler
Inputs: Program with ON ERROR GOTO handler; executes /, \\, and MOD by zero and resumes next
Code under test: end-to-end pipeline division-by-zero error path
Expected behavior: Handler runs three times; prints three lines with "Z"
*/
TEST(E2E, DivisionByZero_TriggersErrorHandler) {
    if (!toolExists(CLANG_PATH)) GTEST_SKIP();
    const char* src =
        "10 ON ERROR GOTO 100\n"
        "20 PRINT 1/0\n"
        "30 PRINT 2\\0\n"
        "40 PRINT 3 MOD 0\n"
        "50 END\n"
        "100 PRINT \"Z\"\n"
        "110 RESUME NEXT\n";
    std::string ir = Compiler::compileString(src);
    std::filesystem::path tmp = std::filesystem::path("..")/"tmp"/"gwbasic_e2e_div_zero";
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
    ASSERT_EQ(lines.size(), 3u);
    EXPECT_EQ(lines[0], std::string("Z"));
    EXPECT_EQ(lines[1], std::string("Z"));
    EXPECT_EQ(lines[2], std::string("Z"));
}

