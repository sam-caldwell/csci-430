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

/*
 * Test Suite: E2E For Loop (sad paths)
 * Purpose: Validate loops with a mismatched direction and bounds do not execute.
 */
/*
Test: E2E.ForLoopSadPath_PositiveStepStartGreaterThanEnd
Inputs: BASIC program(s) executed end-to-end (runtime output)
Code under test: Full compiler pipeline (lexer → parser → semantics → codegen → runtime)
Expected behavior: Program compiles and runs; output/behavior matches expectations
*/
TEST(E2E, ForLoopSadPath_PositiveStepStartGreaterThanEnd) {
    if (!toolExists(CLANG_PATH)) { GTEST_SKIP() << "clang not found, skipping E2E."; }
    std::string src = R"(10 FOR I = 5 TO 1 STEP 1
20 PRINT I
30 NEXT I
40 END
)";
    std::string ir = Compiler::compileString(src);
    std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_for_sad_pos";
    std::filesystem::create_directories(tmp);
    std::filesystem::path ll = tmp / "program.ll";
    std::filesystem::path bin = tmp / "program.out";
    { std::ofstream f(ll); f << ir; }
    std::ostringstream c2; c2 << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    c2 << " -lm";
#endif
    std::string cmd = c2.str();
    int ec = std::system(cmd.c_str());
    ASSERT_EQ(ec, 0);
    std::ostringstream r2; r2 << '"' << bin.string() << '"'; std::string out = runCommand(r2.str());
    ASSERT_EQ(out, "");
}

// moved: ForLoopSadPath_NegativeStepStartLessThanEnd (see test_for_loop_sad_negative_step_e2e.cpp)
/***
 * Test: E2E.ForLoopSadPath_PositiveStepStartGreaterThanEnd
 * Purpose: Validate End-to-End (compile + run) behavior for test_for_loop_step_sad_paths.cpp.
 * Components Under Test: Compiler (compileString/compileFile), Clang driver, runtime output
 * Expected Behavior: Program output matches assertions in test.
 */
