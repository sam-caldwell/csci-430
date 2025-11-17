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
 * Test: E2E.WhileMultiline_CountsUp
 * Purpose: Validate End-to-End (compile + run) behavior for test_while_multiline_e2e.cpp.
 * Components Under Test: Compiler (compileString/compileFile), Clang driver, runtime output
 * Expected Behavior: Program output matches assertions in test.
 */
TEST(E2E, WhileMultiline_CountsUp) {
    if (!toolExists(CLANG_PATH)) { GTEST_SKIP() << "clang not found"; }
    const std::string src =
        "10 LET I = 1\n"
        "20 WHILE I < 4\n"
        "30 PRINT I\n"
        "40 LET I = I + 1\n"
        "50 WEND\n"
        "60 END\n";
    const std::string ir = Compiler::compileString(src);
    const std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_while_multiline";
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
    ASSERT_NE(out.find("1\n2\n3\n"), std::string::npos);
}
