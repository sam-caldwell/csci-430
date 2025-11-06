// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include "basic_compiler/Compiler.h"
#include "clang_path.h"
#include "run_command.h"
#include "../../helper/tool_exists.h"

using namespace gwbasic;
using namespace e2e_helpers;

/***
 * Test: E2E.IfBlock_ThenElse_HighBranch
 * Purpose: Validate End-to-End (compile + run) behavior for test_if_block_then_else_high_e2e.cpp.
 * Components Under Test: Compiler (compileString/compileFile), Clang driver, runtime output
 * Expected Behavior: Program output matches assertions in test.
 */
/*
Test: E2E.IfBlock_ThenElse_HighBranch
Inputs: BASIC program(s) executed end-to-end (runtime output)
Code under test: Full compiler pipeline (lexer → parser → semantics → codegen → runtime)
Expected behavior: Program compiles and runs; output/behavior matches expectations
*/
TEST(E2E, IfBlock_ThenElse_HighBranch) {
    if (!toolExists(CLANG_PATH)) { GTEST_SKIP() << "clang not found"; }
    std::string src =
        "10 LET A = 9\n"
        "20 IF A < 5 THEN\n"
        "30 PRINT \"LOW\"\n"
        "40 ELSE\n"
        "50 PRINT \"HIGH\"\n"
        "60 END IF\n"
        "70 END\n";
    std::string ir = Compiler::compileString(src);
    std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_if_high";
    std::filesystem::create_directories(tmp);
    std::filesystem::path ll = tmp / "program.ll";
    std::filesystem::path bin = tmp / "program.out";
    { std::ofstream f(ll); f << ir; }
    std::ostringstream c2; c2 << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    c2 << " -lm";
#endif
    int ec = std::system(c2.str().c_str());
    ASSERT_EQ(ec, 0);
    std::string out = runCommand(std::string("\"") + bin.string() + "\"");
    ASSERT_NE(out.find("HIGH\n"), std::string::npos);
}

