// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include "basic_compiler/Compiler.h"
#include "../../helper/clang_path.h"
#include "../../helper/run_command.h"
#include "../../helper/tool_exists.h"

using namespace gwbasic;
using namespace e2e_helpers;

/***
 * Test: E2E.ForLoopSadPath_NegativeStepStartLessThanEnd
 * Purpose: Validate End-to-End (compile + run) behavior for test_for_loop_sad_negative_step_e2e.cpp.
 * Components Under Test: Compiler (compileString/compileFile), Clang driver, runtime output
 * Expected Behavior: Program output matches assertions in test.
 */
TEST(E2E, ForLoopSadPath_NegativeStepStartLessThanEnd) {
    if (!toolExists(CLANG_PATH)) { GTEST_SKIP() << "clang not found, skipping E2E."; }
    std::string src = R"(10 FOR I = 1 TO 5 STEP -1
20 PRINT I
30 NEXT I
40 END
)";
    const std::string ir = Compiler::compileString(src);
    const std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_for_sad_neg";
    std::filesystem::create_directories(tmp);
    const std::filesystem::path ll = tmp / "program.ll";
    const std::filesystem::path bin = tmp / "program.out";
    { std::ofstream f(ll); f << ir; }
    std::ostringstream c2; c2 << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    c2 << " -lm";
#endif
    std::string cmd = c2.str();
    const int ec = std::system(cmd.c_str());
    ASSERT_EQ(ec, 0);
    std::ostringstream r2; r2 << '"' << bin.string() << '"'; const std::string out = runCommand(r2.str());
    ASSERT_EQ(out, "");
}
