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
 * Test: E2E.READ_Restore_RewindsData
 * Purpose: Verify RESTORE rewinds DATA so subsequent READ returns from the start.
 * Components Under Test: DATA/READ tables and RESTORE effect; End-to-End run.
 * Expected Behavior: Output sequence reads the same items again after RESTORE.
 */
TEST(E2E, READ_Restore_RewindsData) {
    if (!toolExists(CLANG_PATH)) {
        GTEST_SKIP() << "clang not found (CLANG_PATH='" << CLANG_PATH << "'), skipping E2E.";
    }
    // Read two items, RESTORE, and read again; expect A then B then A then B
    const char* src =
        "10 DATA \"A\",\"B\"\n"
        "20 READ S$, T$\n"
        "30 PRINT S$\n"
        "40 PRINT T$\n"
        "50 RESTORE\n"
        "60 READ S$, T$\n"
        "70 PRINT S$\n"
        "80 PRINT T$\n";
    const std::string ir = Compiler::compileString(src);
    const std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_read_restore";
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
    // Expect four lines: A, B, A, B
    const size_t p1 = out.find("A\n"); ASSERT_NE(p1, std::string::npos);
    const size_t p2 = out.find("B\n", p1 + 2); ASSERT_NE(p2, std::string::npos);
    const size_t p3 = out.find("A\n", p2 + 2); ASSERT_NE(p3, std::string::npos);
    const size_t p4 = out.find("B\n", p3 + 2); ASSERT_NE(p4, std::string::npos);
}
