// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include "basic_compiler/Compiler.h"
#include "clang_path.h"
#include "run_command.h"
#include "../helper/tool_exists.h"

using namespace gwbasic;
using namespace e2e_helpers;

/*
 * Test: E2E.SCREEN_ReturnsAsciiAfterPrint
 * Inputs: Program that prints "ABC" then prints SCREEN(1,1..3).
 * Code under test: Virtual screen buffer + SCREEN() runtime semantics E2E.
 * Expected behavior: Outputs 65,66,67 after printing ABC (ASCII for A,B,C).
 */
TEST(E2E, SCREEN_ReturnsAsciiAfterPrint) {
    if (!toolExists(CLANG_PATH)) {
        GTEST_SKIP() << "clang not found (CLANG_PATH='" << CLANG_PATH << "'), skipping E2E.";
    }
    const char* src =
        "10 PRINT \"ABC\"\n"
        "20 PRINT SCREEN(1,1), SCREEN(1,2), SCREEN(1,3)\n"
        "30 END\n";
    std::string ir = Compiler::compileString(src);
    ASSERT_FALSE(ir.empty());
    std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_screen";
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
    // Look for ASCII codes 65, 66, 67 (as doubles with default precision)
    ASSERT_NE(out.find("65.000000"), std::string::npos);
    ASSERT_NE(out.find("66.000000"), std::string::npos);
    ASSERT_NE(out.find("67.000000"), std::string::npos);
}
