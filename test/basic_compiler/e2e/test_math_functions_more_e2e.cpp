// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>
#include "basic_compiler/Compiler.h"
#include "clang_path.h"
#include "run_command.h"
#include "../helper/tool_exists.h"
#include "../helper/split_lines.h"

using namespace gwbasic;
using namespace e2e_helpers;

/***
 * Test: E2E.MoreMathIntrinsicsWork
 * Purpose: Validate End-to-End (compile + run) behavior for test_math_functions_more_e2e.cpp.
 * Components Under Test: Compiler (compileString/compileFile), Clang driver, runtime output
 * Expected Behavior: Program output matches assertions in test.
 */
TEST(E2E, MoreMathIntrinsicsWork) {
    if (!toolExists(CLANG_PATH)) {
        GTEST_SKIP() << "clang not found (CLANG_PATH='" << CLANG_PATH << "'), skipping E2E.";
    }
    const auto src =
        "10 PRINT CINT(1.6)\n"
        "20 PRINT CINT(-1.6)\n"
        "30 PRINT CDBL(1.5)\n"
        "40 PRINT CSNG(1.5)\n"
        "50 PRINT RND(1)\n"
        "60 END\n";

    std::string ir = Compiler::compileString(src);

    std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_math_more";
    std::filesystem::create_directories(tmp);
    std::filesystem::path ll = tmp / "program.ll";
    std::filesystem::path bin = tmp / "program.out";
    { std::ofstream f(ll); f << ir; }

    std::ostringstream cmd; cmd << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    cmd << " -lm";
#endif
    int ec = std::system(cmd.str().c_str());
    ASSERT_EQ(ec, 0);

    std::ostringstream run; run << '"' << bin.string() << '"';
    std::string out = runCommand(run.str());
    auto lines = splitLines(out);
    ASSERT_EQ(lines.size(), 5u);

    EXPECT_EQ(lines[0], std::string("2.000000"));     // CINT(1.6)
    EXPECT_EQ(lines[1], std::string("-2.000000"));    // CINT(-1.6)
    EXPECT_EQ(lines[2], std::string("1.500000"));     // CDBL(1.5)
    EXPECT_EQ(lines[3], std::string("1.500000"));     // CSNG(1.5)
    // RND(1): 0.0 <= x < 1.0 (value varies)
    {
        double v = std::stod(lines[4]);
        EXPECT_GE(v, 0.0);
        EXPECT_LT(v, 1.0);
    }
}