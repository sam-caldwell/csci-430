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
#include "../../helper/split_lines.h"

using namespace gwbasic;
using namespace e2e_helpers;

/***
 * Test: E2E.ArrayBoundsTriggersOnErrorHandler
 * Purpose: Confirm out-of-bounds array write triggers ON ERROR handler.
 * Components Under Test: Codegen bounds checks, error flagging, handler dispatch, End-to-End run.
 * Expected Behavior: Only handler value (999) is printed.
 */
TEST(E2E, ArrayBoundsTriggersOnErrorHandler) {
    if (!toolExists(CLANG_PATH)) GTEST_SKIP();
    const char* src =
        "10 DIM A(2)\n"
        "20 ON ERROR GOTO 100\n"
        "30 A(-1)=3\n"
        "40 PRINT 111\n"
        "100 PRINT 999\n"
        "110 END\n";
    std::string ir = Compiler::compileString(src);
    std::filesystem::path tmp = std::filesystem::path("..")/"tmp"/"gwbasic_e2e_bounds";
    std::filesystem::create_directories(tmp);
    auto ll = tmp/"program.ll"; auto bin = tmp/"program.out";
    { std::ofstream f(ll); f << ir; }
    std::ostringstream cmd; cmd << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    cmd << " -lm";
#endif
    ASSERT_EQ(std::system(cmd.str().c_str()), 0);
    std::string out = runCommand(std::string("\"") + bin.string() + "\"");
    auto lines = splitLines(out);
    ASSERT_EQ(lines.size(), 1u);
    EXPECT_EQ(lines[0], std::string("999"));
}
