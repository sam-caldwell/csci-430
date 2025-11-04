// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include "basic_compiler/Compiler.h"
#include "clang_path.h"
#include "run_command.h"
#include "../helper/tool_exists.h"

using namespace gwbasic;
using namespace e2e_helpers;

/***
 * Test: E2E.WhileInline_CountsUp
 * Purpose: Validate End-to-End (compile + run) behavior for test_while_inline_e2e.cpp.
 * Components Under Test: Compiler (compileString/compileFile), Clang driver, runtime output
 * Expected Behavior: Program output matches assertions in test.
 */
/*
Test: E2E.WhileInline_CountsUp
Inputs: BASIC program(s) executed end-to-end (runtime output)
Code under test: Full compiler pipeline (lexer → parser → semantics → codegen → runtime)
Expected behavior: Program compiles and runs; output/behavior matches expectations
*/
TEST(E2E, WhileInline_CountsUp) {
    if (!toolExists(CLANG_PATH)) { GTEST_SKIP() << "clang not found"; }
    std::string src =
        "10 LET I = 1: WHILE I < 4: PRINT I: LET I = I + 1: WEND\n"
        "20 END\n";
    std::string ir = Compiler::compileString(src);
    std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_while_inline";
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
    ASSERT_NE(out.find("1.000000\n2.000000\n3.000000\n"), std::string::npos);
}

