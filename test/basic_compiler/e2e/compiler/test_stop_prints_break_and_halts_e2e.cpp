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
 * Test: E2E.StopPrintsBreakAndHalts
 * Purpose: Verify STOP prints a break message and halts program execution.
 * Components Under Test: Codegen STOP handler (break message), control flow, End-to-End run.
 * Expected Behavior: Output contains 1 and "Break in 20" and does not contain 2.
 */
TEST(E2E, StopPrintsBreakAndHalts) {
    if (!toolExists(CLANG_PATH)) GTEST_SKIP() << "clang not found (CLANG_PATH='" << CLANG_PATH << "')";
    std::string src = R"(10 PRINT 1
20 STOP
30 PRINT 2
)";
    const std::string ir = Compiler::compileString(src);
    const std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_stop";
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
    const std::string out = runCommand(run.str());
    // Expect 1, then Break in 20, and no 2
    size_t p1 = out.find("1\n"); ASSERT_NE(p1, std::string::npos);
    size_t pb = out.find("Break in 20\n", p1 + 1); ASSERT_NE(pb, std::string::npos);
    ASSERT_EQ(out.find("2\n", pb + 1), std::string::npos);
}
