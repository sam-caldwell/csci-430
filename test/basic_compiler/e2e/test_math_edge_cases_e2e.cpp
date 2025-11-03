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

using namespace gwbasic;
using namespace e2e_helpers;

/***
 * Test: E2E.MathEdgeCasesProduceExpectedResults
 * Purpose: Validate End-to-End (compile + run) behavior for test_math_edge_cases_e2e.cpp.
 * Components Under Test: Compiler (compileString/compileFile), Clang driver, runtime output
 * Expected Behavior: Program output matches assertions in test.
 */
TEST(E2E, MathEdgeCasesProduceExpectedResults) {
    if (!toolExists(CLANG_PATH)) {
        GTEST_SKIP() << "clang not found (CLANG_PATH='" << CLANG_PATH << "'), skipping E2E.";
    }
    // Choose values with stable 6-decimal outputs
    const auto src =
        "10 PRINT SQR(0.0001)\n"     // 0.01
        "20 PRINT LOG(0.001)\n"      // -6.907755
        "30 PRINT EXP(5)\n"          // 148.413159
        "40 PRINT INT(-1.1)\n"       // -2
        "50 PRINT FIX(-1.1)\n"       // -1
        "60 PRINT SGN(0.000001)\n"   // 1
        "70 END\n";

    std::string ir = Compiler::compileString(src);

    std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_math_edges";
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

    const std::string expected =
        "0.010000\n"
        "-6.907755\n"
        "148.413159\n"
        "-2.000000\n"
        "-1.000000\n"
        "1.000000\n";
    ASSERT_EQ(out, expected);
}