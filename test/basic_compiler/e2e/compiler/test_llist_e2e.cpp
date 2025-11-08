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
 * Test: E2E.LLIST_PrintsLineNumbersOrFallback
 * Purpose: Compile and run a program with LLIST; verify it produces line
 *          numbers. When no printer channel is open, LLIST safely falls back
 *          to stdout + screen mirror.
 */
TEST(E2E, LLIST_PrintsLineNumbersOrFallback) {
    if (!toolExists(CLANG_PATH)) {
        GTEST_SKIP() << "clang not found (CLANG_PATH='" << CLANG_PATH << "'), skipping E2E.";
    }
    const char* src =
        "10 PRINT \"Z\"\n"
        "20 LLIST\n"
        "30 END\n";
    std::string ir = Compiler::compileString(src);
    ASSERT_FALSE(ir.empty());
    std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_llist";
    std::filesystem::create_directories(tmp);
    auto ll = tmp/"program.ll";
    auto bin = tmp/"program.out";
    { std::ofstream f(ll); f << ir; }
    std::ostringstream c1; c1 << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    c1 << " -lm";
#endif
    int ec = std::system(c1.str().c_str());
    ASSERT_EQ(ec, 0);
    std::string out = runCommand(std::string("\"") + bin.string() + "\"");
    // Expect line numbers in output (printed or mirrored)
    ASSERT_NE(out.find("10\n"), std::string::npos);
    ASSERT_NE(out.find("20\n"), std::string::npos);
    ASSERT_NE(out.find("30\n"), std::string::npos);
}

