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
 * Test: E2E.OnErrorGotoAndResumeNext
 * Purpose: Validate ON ERROR GOTO handler executes and RESUME NEXT continues after the faulting statement.
 * Components Under Test: Parser/semantics for error handling, Codegen switch/handler flags, End-to-End run.
 * Expected Behavior: Output sequence is 1, handler print, 2, 3 with no crash.
 */
TEST(E2E, OnErrorGotoAndResumeNext) {
    if (!toolExists(CLANG_PATH)) {
        GTEST_SKIP() << "clang not found (CLANG_PATH='" << CLANG_PATH << "'), skipping E2E.";
    }
    std::string src = R"(10 ON ERROR GOTO 300
20 PRINT 1
30 ERROR 5
40 PRINT 2
300 PRINT "H"
310 RESUME NEXT
320 PRINT 3
)";
    std::string ir = Compiler::compileString(src);
    std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_on_error_resume";
    std::filesystem::create_directories(tmp);
    auto ll = tmp / "program.ll";
    auto bin = tmp / "program.out";
    { std::ofstream f(ll); f << ir; }
    std::ostringstream cmd; cmd << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    cmd << " -lm";
#endif
    ASSERT_EQ(std::system(cmd.str().c_str()), 0);
    std::ostringstream run; run << '"' << bin.string() << '"';
    std::string out = runCommand(run.str());
    // Expect the handler line to print H, and overall sequence 1, H, 2, 3
    size_t p1 = out.find("1\n"); ASSERT_NE(p1, std::string::npos);
    size_t pH = out.find("H\n", p1 + 1); ASSERT_NE(pH, std::string::npos);
    size_t p2 = out.find("2\n", pH + 1); ASSERT_NE(p2, std::string::npos);
    size_t p3 = out.find("3\n", p2 + 1); ASSERT_NE(p3, std::string::npos);
}
