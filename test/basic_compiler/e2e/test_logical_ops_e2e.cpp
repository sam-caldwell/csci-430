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
Test: E2E.LogicalOps_Truthiness
Inputs: Program printing logical AND/OR/NOT with numeric truthiness
Expected behavior: Correct 0/1 results for combinations.
*/
TEST(E2E, LogicalOps_Truthiness) {
    if (!toolExists(CLANG_PATH)) {
        GTEST_SKIP() << "clang not found (CLANG_PATH='" << CLANG_PATH << "'), skipping E2E.";
    }
    const auto src =
        "10 PRINT 1 AND 0\n"
        "20 PRINT 2 AND 3\n"
        "30 PRINT 0 OR 5\n"
        "40 PRINT 0 OR 0\n"
        "50 PRINT NOT 0\n"
        "60 PRINT NOT 2\n"
        "70 END\n";

    std::string ir = Compiler::compileString(src);
    std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_logical";
    std::filesystem::create_directories(tmp);
    std::filesystem::path ll = tmp / "program.ll";
    std::filesystem::path bin = tmp / "program.out";
    { std::ofstream f(ll); f << ir; }

    std::ostringstream cmd; cmd << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    cmd << " -lm";
#endif
    int ec = std::system(cmd.str().c_str());
    ASSERT_EQ(ec, 0);

    std::ostringstream run; run << '"' << bin.string() << '"';
    std::string out = runCommand(run.str());

    const std::string expected =
        "0\n"
        "1\n"
        "1\n"
        "0\n"
        "1\n"
        "0\n";
    ASSERT_EQ(out, expected);
}

