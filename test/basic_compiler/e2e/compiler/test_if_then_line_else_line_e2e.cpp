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

/***
Test: E2E.If_ThenLine_ElseLine_Branches
Inputs: Program with IF A=0 THEN 100 ELSE 200, two runs with A=0 and A=1
Expected behavior: Outputs 1 for A=0 and 2 for A=1
*/
TEST(E2E, If_ThenLine_ElseLine_Branches) {
    if (!toolExists(CLANG_PATH)) { GTEST_SKIP() << "clang not found"; }
    auto buildRun = [](int a) -> std::string {
        std::ostringstream src;
        src << "10 A=" << a << "\n";
        src << "20 IF A=0 THEN 100 ELSE 200\n";
        src << "30 END\n";
        src << "100 PRINT 1\n";
        src << "200 PRINT 2\n";
        const std::string ir = Compiler::compileString(src.str());
        const std::filesystem::path tmp = std::filesystem::path("..")/"tmp"/"gwbasic_e2e_if_then_else_line";
        std::filesystem::create_directories(tmp);
        auto ll = tmp/ (std::string("p_") + (a==0?"t":"f") + ".ll");
        auto bin = tmp/ (std::string("p_") + (a==0?"t":"f") + ".out");
        { std::ofstream f(ll); f << ir; }
        std::ostringstream cmd; cmd << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
        cmd << " -lm";
#endif
        EXPECT_EQ(std::system(cmd.str().c_str()), 0);
        return runCommand(std::string("\"") + bin.string() + "\"");
    };
    auto out0 = buildRun(0);
    ASSERT_NE(out0.find("1"), std::string::npos);
    auto out1 = buildRun(1);
    ASSERT_NE(out1.find("2"), std::string::npos);
}
