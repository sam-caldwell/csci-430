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
 * Test: E2E.OnGotoAndOnGosub
 * Purpose: Validate ON GOTO dispatches to correct target and ON GOSUB executes subroutine then continues.
 * Components Under Test: Parser/semantics for ON statements, Codegen switch/inline subroutine, End-to-End run.
 * Expected Behavior: ON GOTO prints only 200 (no 999); ON GOSUB prints sub1 then done.
 */
TEST(E2E, OnGotoAndOnGosub) {
    if (!toolExists(CLANG_PATH)) {
        GTEST_SKIP() << "clang not found (CLANG_PATH='" << CLANG_PATH << "'), skipping E2E.";
    }
    // ON GOTO: index 2 should jump to 200 and not print X
    {
        std::string src = R"(10 A=2: ON A GOTO 100,200: PRINT 999: END
100 PRINT 100: END
200 PRINT 200: END
)";
        std::string ir = Compiler::compileString(src);
        std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_on_goto";
        std::filesystem::create_directories(tmp);
        std::filesystem::path ll = tmp / "program.ll";
        std::filesystem::path bin = tmp / "program.out";
        { std::ofstream f(ll); f << ir; }
        std::ostringstream c3; c3 << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
        c3 << " -lm";
#endif
        int ec = std::system(c3.str().c_str());
        ASSERT_EQ(ec, 0);
        std::ostringstream r3; r3 << '"' << bin.string() << '"'; std::string out = runCommand(r3.str());
        ASSERT_NE(out.find("200"), std::string::npos);
        ASSERT_EQ(out.find("999"), std::string::npos);
    }
    // ON GOSUB: index 1 should execute sub then continue to print main
    {
        std::string src = R"(10 N=1: ON N GOSUB 100,200: PRINT "done": END
100 PRINT "sub1": RETURN
200 PRINT "sub2": RETURN
)";
        std::string ir = Compiler::compileString(src);
        std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_on_gosub";
        std::filesystem::create_directories(tmp);
        std::filesystem::path ll = tmp / "program.ll";
        std::filesystem::path bin = tmp / "program.out";
        { std::ofstream f(ll); f << ir; }
        std::ostringstream c3; c3 << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
        c3 << " -lm";
#endif
        int ec = std::system(c3.str().c_str());
        ASSERT_EQ(ec, 0);
        std::ostringstream r3; r3 << '"' << bin.string() << '"'; std::string out = runCommand(r3.str());
        size_t p1 = out.find("sub1\n"); ASSERT_NE(p1, std::string::npos);
        size_t p2 = out.find("done\n", p1 + 1); ASSERT_NE(p2, std::string::npos);
    }
}
