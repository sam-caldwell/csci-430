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

/*
Test: E2E.OnGotoAndOnGosub
Inputs: BASIC program(s) executed end-to-end
Code under test: Full pipeline + runtime
Expected behavior: ON GOTO dispatches to correct target; ON GOSUB inlines and returns
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

/*
Test: E2E.OnGotoAndOnGosub_OutOfRange
Inputs: Programs with ON index=0 and index>N
Expected behavior: No branch/subroutine taken; falls through to next statement
*/
TEST(E2E, OnGotoAndOnGosub_OutOfRange) {
    if (!toolExists(CLANG_PATH)) {
        GTEST_SKIP() << "clang not found (CLANG_PATH='" << CLANG_PATH << "'), skipping E2E.";
    }
    auto buildAndRun = [](const std::string& name, const std::string& src) -> std::string {
        std::string ir = Compiler::compileString(src);
        std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / ("gwbasic_e2e_on_out_" + name);
        std::filesystem::create_directories(tmp);
        auto ll = tmp / "program.ll";
        auto bin = tmp / "program.out";
        { std::ofstream f(ll); f << ir; }
        std::ostringstream cmd; cmd << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
        cmd << " -lm";
#endif
        EXPECT_EQ(std::system(cmd.str().c_str()), 0);
        std::ostringstream run; run << '"' << bin.string() << '"';
        return runCommand(run.str());
    };

    // ON GOTO: index 0 -> falls through; prints 777
    {
        std::string src = R"(10 A=0: ON A GOTO 100,200: PRINT 777: END
100 PRINT 100: END
200 PRINT 200: END
)";
        auto out = buildAndRun("goto0", src);
        ASSERT_NE(out.find("777"), std::string::npos);
        ASSERT_EQ(out.find("100\n"), std::string::npos);
        ASSERT_EQ(out.find("200\n"), std::string::npos);
    }
    // ON GOTO: index >N -> falls through; prints 777
    {
        std::string src = R"(10 A=3: ON A GOTO 100,200: PRINT 777: END
100 PRINT 100: END
200 PRINT 200: END
)";
        auto out = buildAndRun("goto3", src);
        ASSERT_NE(out.find("777"), std::string::npos);
        ASSERT_EQ(out.find("100\n"), std::string::npos);
        ASSERT_EQ(out.find("200\n"), std::string::npos);
    }
    // ON GOSUB: index 0 -> no sub called; prints only done
    {
        std::string src = R"(10 N=0: ON N GOSUB 100,200: PRINT "done": END
100 PRINT "sub1": RETURN
200 PRINT "sub2": RETURN
)";
        auto out = buildAndRun("gosub0", src);
        ASSERT_NE(out.find("done"), std::string::npos);
        ASSERT_EQ(out.find("sub1"), std::string::npos);
        ASSERT_EQ(out.find("sub2"), std::string::npos);
    }
    // ON GOSUB: index >N -> no sub called; prints only done
    {
        std::string src = R"(10 N=9: ON N GOSUB 100,200: PRINT "done": END
100 PRINT "sub1": RETURN
200 PRINT "sub2": RETURN
)";
        auto out = buildAndRun("gosub9", src);
        ASSERT_NE(out.find("done"), std::string::npos);
        ASSERT_EQ(out.find("sub1"), std::string::npos);
        ASSERT_EQ(out.find("sub2"), std::string::npos);
    }
}
