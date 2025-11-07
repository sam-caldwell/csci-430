// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include "basic_compiler/Compiler.h"
#include "clang_path.h"
#include "../../helper/run_command.h"
#include "../../helper/tool_exists.h"

using namespace gwbasic;
using namespace e2e_helpers;

/***
 * Test: E2E.OnGotoAndOnGosub_OutOfRange
 * Purpose: Verify out-of-range or zero index for ON dispatch falls through to next statement.
 * Components Under Test: Codegen default switch labels for ON GOTO/GOSUB, End-to-End run.
 * Expected Behavior: Programs with invalid indices print the fallthrough text only.
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
