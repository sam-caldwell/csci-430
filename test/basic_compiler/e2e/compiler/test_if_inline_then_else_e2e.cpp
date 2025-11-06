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

using namespace gwbasic;
using namespace e2e_helpers;

/***
 * Test: E2E.IfInlineThenElseTrueFalse
 * Purpose: Validate runtime behavior of inline IF THEN/ELSE lists on both branches.
 */
/*
Test: E2E.IfInlineThenElseTrueFalse
Inputs: Two programs: one true branch (prints T), one false branch (prints F)
Code under test: Full pipeline + runtime
Expected behavior: Outputs contain expected letter
*/
TEST(E2E, IfInlineThenElseTrueFalse) {
    if (!toolExists(CLANG_PATH)) { GTEST_SKIP() << "clang not found"; }
    // True branch
    {
        const std::string src =
            "10 A=1\n"
            "20 IF A=1 THEN PRINT \"T\" ELSE PRINT \"F\"\n"
            "30 END\n";
        std::string ir = Compiler::compileString(src);
        std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_if_inline_true";
        std::filesystem::create_directories(tmp);
        auto ll = tmp / "p.ll"; auto bin = tmp / "p.out"; { std::ofstream f(ll); f << ir; }
        std::ostringstream cmd; cmd << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
        cmd << " -lm";
#endif
        ASSERT_EQ(std::system(cmd.str().c_str()), 0);
        std::string out = runCommand(std::string("\"") + bin.string() + "\"");
        ASSERT_NE(out.find("T"), std::string::npos);
    }
    // False branch
    {
        const std::string src =
            "10 A=0\n"
            "20 IF A=1 THEN PRINT \"T\" ELSE PRINT \"F\"\n"
            "30 END\n";
        std::string ir = Compiler::compileString(src);
        std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_if_inline_false";
        std::filesystem::create_directories(tmp);
        auto ll = tmp / "p.ll"; auto bin = tmp / "p.out"; { std::ofstream f(ll); f << ir; }
        std::ostringstream cmd; cmd << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
        cmd << " -lm";
#endif
        ASSERT_EQ(std::system(cmd.str().c_str()), 0);
        std::string out = runCommand(std::string("\"") + bin.string() + "\"");
        ASSERT_NE(out.find("F"), std::string::npos);
    }
}

