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
 * Test: E2E.IfBlock_ThenElse_LowBranch
 * Purpose: Validate End-to-End (compile + run) behavior for test_if_block_then_else_e2e.cpp.
 * Components Under Test: Compiler (compileString/compileFile), Clang driver, runtime output
 * Expected Behavior: Program output matches assertions in test.
 */
TEST(E2E, IfBlock_ThenElse_LowBranch) {
    if (!toolExists(CLANG_PATH)) { GTEST_SKIP() << "clang not found"; }
    std::string src =
        "10 LET A = 3\n"
        "20 IF A < 5 THEN\n"
        "30 PRINT \"LOW\"\n"
        "40 ELSE\n"
        "50 PRINT \"HIGH\"\n"
        "60 END IF\n"
        "70 END\n";
    const std::string ir = Compiler::compileString(src);
    const std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_if_low";
    std::filesystem::create_directories(tmp);
    const std::filesystem::path ll = tmp / "program.ll";
    const std::filesystem::path bin = tmp / "program.out";
    { std::ofstream f(ll); f << ir; }
    std::ostringstream c2; c2 << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    c2 << " -lm";
#endif
    const int ec = std::system(c2.str().c_str());
    ASSERT_EQ(ec, 0);
    const std::string out = runCommand(std::string("\"") + bin.string() + "\"");
    ASSERT_NE(out.find("LOW\n"), std::string::npos);
}
