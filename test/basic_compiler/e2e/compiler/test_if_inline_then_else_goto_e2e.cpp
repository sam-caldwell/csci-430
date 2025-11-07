// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include "basic_compiler/Compiler.h"
#include "../../helper/clang_path.h"
#include "../../helper/run_command.h"
#include "../../helper/tool_exists.h"

using namespace gwbasic;
using namespace e2e_helpers;

/*
 * Test: E2E.IfInlineThenElseGoto_Branches
 * Inputs: Program uses inline THEN GOTO and ELSE GOTO; runs two variants to cover both
 * Code under test: Full pipeline + runtime
 * Expected behavior: Output matches branch target
*/
TEST(E2E, IfInlineThenElseGoto_Branches) {
    if (!toolExists(CLANG_PATH)) { GTEST_SKIP() << "clang not found"; }
    auto buildAndRun = [](int a) -> std::string {
        std::ostringstream src;
        src << "10 A=" << a << "\n";
        src << "20 IF A=1 THEN GOTO 100 ELSE GOTO 200\n";
        src << "30 END\n";
        src << "100 PRINT 1\n";
        src << "200 PRINT 2\n";
        const std::string s = src.str();
        std::string ir = Compiler::compileString(s);
        std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_if_inline_goto";
        std::filesystem::create_directories(tmp);
        auto ll = tmp / (std::string("p_") + (a==1?"t":"f") + ".ll");
        auto bin = tmp / (std::string("p_") + (a==1?"t":"f") + ".out");
        { std::ofstream f(ll); f << ir; }
        std::ostringstream cmd; cmd << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
        cmd << " -lm";
#endif
        EXPECT_EQ(std::system(cmd.str().c_str()), 0);
        return runCommand(std::string("\"") + bin.string() + "\"");
    };
    auto out1 = buildAndRun(1);
    ASSERT_NE(out1.find("1"), std::string::npos);
    auto out0 = buildAndRun(0);
    ASSERT_NE(out0.find("2"), std::string::npos);
}

