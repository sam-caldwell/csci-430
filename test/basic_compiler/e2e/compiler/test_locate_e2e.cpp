// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"
#include "../../helper/clang_path.h"
#include "../../helper/run_command.h"
#include "../../helper/tool_exists.h"

using namespace gwbasic;
using namespace e2e_helpers;

/***
 * Test: E2E.LOCATE_SetsCursorForPrint
 * Purpose: Move cursor to row 2, col 3 and print "X". SCREEN(2,3) returns ASCII of 'X'.
 */
TEST(E2E, LOCATE_SetsCursorForPrint) {
    if (!toolExists(CLANG_PATH)) { GTEST_SKIP() << "clang not found"; }
    const std::string src =
        "10 LOCATE 2,3\n"
        "20 PRINT \"X\"\n"
        "30 PRINT SCREEN(2,3)\n";
    const std::string ir = Compiler::compileString(src.c_str());
    const std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_locate";
    std::filesystem::create_directories(tmp);
    auto ll = tmp / "p.ll"; auto bin = tmp / "p.out"; { std::ofstream f(ll); f << ir; }
    std::ostringstream cmd; cmd << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    cmd << " -lm";
#endif
    ASSERT_EQ(std::system(cmd.str().c_str()), 0);
    const std::string out = runCommand(std::string("\"") + bin.string() + "\"");
    // ASCII for 'X' is 88; expect to see 88 printed
    ASSERT_NE(out.find("88"), std::string::npos);
}

