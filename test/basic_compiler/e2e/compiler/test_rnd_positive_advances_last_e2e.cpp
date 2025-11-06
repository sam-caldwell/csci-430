// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "basic_compiler/Compiler.h"
#include "clang_path.h"
#include "run_command.h"
#include "../../helper/tool_exists.h"
#include "../../helper/split_lines.h"

using namespace gwbasic;
using namespace e2e_helpers;

/***
 * Test: E2E.RndPositiveAdvancesLast
 * Purpose: Validate End-to-End (compile + run) behavior for test_rnd_positive_advances_last_e2e.cpp.
 * Components Under Test: Compiler (compileString/compileFile), Clang driver, runtime output
 * Expected Behavior: Program output matches assertions in test.
 */
/*
Test: E2E.RndPositiveAdvancesLast
Inputs: BASIC program(s) executed end-to-end (runtime output)
Code under test: Full compiler pipeline (lexer → parser → semantics → codegen → runtime)
Expected behavior: Program compiles and runs; output/behavior matches expectations
*/
TEST(E2E, RndPositiveAdvancesLast) {
    if (!toolExists(CLANG_PATH)) { GTEST_SKIP() << "clang not found"; }
    const auto src =
        "10 RANDOMIZE 99\n"
        "20 A = RND(1)\n"
        "30 PRINT A\n"
        "40 B = RND(1)\n"
        "50 PRINT B\n"
        "60 PRINT RND(0)\n"
        "70 END\n";
    std::string ir = Compiler::compileString(src);
    std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_rnd_pos";
    std::filesystem::create_directories(tmp);
    auto ll = tmp / "program.ll"; auto bin = tmp / "program.out"; { std::ofstream f(ll); f << ir; }
    std::ostringstream cmd; cmd << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    cmd << " -lm";
#endif
    ASSERT_EQ(std::system(cmd.str().c_str()), 0);
    std::string out = runCommand(std::string("\"") + bin.string() + "\"");
    auto lines = splitLines(out);
    ASSERT_EQ(lines.size(), 3u);
    EXPECT_NE(lines[0], lines[1]);
    EXPECT_EQ(lines[1], lines[2]);
}