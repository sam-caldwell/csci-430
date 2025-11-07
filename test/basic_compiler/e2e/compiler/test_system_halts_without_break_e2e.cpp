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
 * Test: E2E.SystemHaltsWithoutBreak
 * Purpose: Ensure SYSTEM halts program without printing break message.
 * Components Under Test: Codegen SYSTEM handler, End-to-End run.
 * Expected Behavior: Output contains only the pre-SYSTEM line and no break message.
 */
TEST(E2E, SystemHaltsWithoutBreak) {
    if (!toolExists(CLANG_PATH)) GTEST_SKIP() << "clang not found (CLANG_PATH='" << CLANG_PATH << "')";
    std::string src = R"(10 PRINT 1
20 SYSTEM
30 PRINT 2
)";
    std::string ir = Compiler::compileString(src);
    std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_system";
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
    // Expect only 1 and no Break message and no 2
    size_t p1 = out.find("1\n"); ASSERT_NE(p1, std::string::npos);
    ASSERT_EQ(out.find("Break in"), std::string::npos);
    ASSERT_EQ(out.find("2\n", p1 + 1), std::string::npos);
}
