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
 * Test: E2E.ResumeLine_FlowsToExplicitTarget
 * Inputs: Program triggers handler and RESUME 30
 * Code under test: End-to-end RESUME line behavior
 * Expected behavior: Output order: 1, H, 3 (skips 2 on error path)
 */
TEST(E2E, ResumeLine_FlowsToExplicitTarget) {
    if (!toolExists(CLANG_PATH)) {
        GTEST_SKIP() << "clang not found (CLANG_PATH='" << CLANG_PATH << "'), skipping E2E.";
    }
    const char* src =
        "10 ON ERROR GOTO 100\n"
        "20 PRINT 1: ERROR 5: PRINT 2\n"
        "30 PRINT 3\n"
        "100 PRINT \"H\"\n"
        "110 RESUME 30\n";
    const std::string ir = Compiler::compileString(src);
    ASSERT_FALSE(ir.empty());
    const std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_resume_line";
    std::filesystem::create_directories(tmp);
    auto ll = tmp / "program.ll"; auto bin = tmp / "program.out";
    { std::ofstream f(ll); f << ir; }
    std::ostringstream cmd; cmd << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    cmd << " -lm";
#endif
    ASSERT_EQ(std::system(cmd.str().c_str()), 0);
    std::ostringstream run; run << '"' << bin.string() << '"';
    const std::string out = runCommand(run.str());
    // Expect to see 1, then H, then 3; and not 2
    size_t p1 = out.find("1\n"); ASSERT_NE(p1, std::string::npos);
    size_t pH = out.find("H\n", p1+1); ASSERT_NE(pH, std::string::npos);
    size_t p3 = out.find("3\n", pH+1); ASSERT_NE(p3, std::string::npos);
    ASSERT_EQ(out.find("2\n"), std::string::npos);
}
