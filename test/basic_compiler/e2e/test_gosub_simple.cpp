// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include "basic_compiler/Compiler.h"
#include "clang_path.h"
#include "run_command.h"
#include "tool_exists.h"

using namespace gwbasic;
using namespace e2e_helpers;
/*
 * Test Suite: E2E Gosub Simple
 * Purpose: Validate inlined GOSUB/RETURN sequencing end-to-end.
 * Components Under Test: Full compiler pipeline; GOSUB inlining; clang.
 * Expected Behavior: Output order is main1, sub, main2 as printed by program.
 */
TEST(E2E, GosubSimple) {
    if (!toolExists(CLANG_PATH)) {
        GTEST_SKIP() << "clang not found (CLANG_PATH='" << CLANG_PATH << "'), skipping E2E.";
    }
    std::string src = R"(10 PRINT "main1"
20 GOSUB 100 : PRINT "main2" : END
100 PRINT "sub" : RETURN
)";
    std::string ir = Compiler::compileString(src);

    std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_gosub";
    std::filesystem::create_directories(tmp);
    std::filesystem::path ll = tmp / "program.ll";
    std::filesystem::path bin = tmp / "program.out";
    { std::ofstream f(ll); f << ir; }

    std::ostringstream c3; c3 << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\""; std::string cmd = c3.str();
    int ec = std::system(cmd.c_str());
    ASSERT_EQ(ec, 0);
    std::ostringstream r3; r3 << '"' << bin.string() << '"'; std::string out = runCommand(r3.str());
    size_t p1 = out.find("main1\n"); ASSERT_NE(p1, std::string::npos);
    size_t p2 = out.find("sub\n", p1 + 1); ASSERT_NE(p2, std::string::npos);
    size_t p3 = out.find("main2\n", p2 + 1); ASSERT_NE(p3, std::string::npos);
}
