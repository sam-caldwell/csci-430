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
Test: E2E.RelationalOps_Numeric_And_String
Inputs: Program printing results of numeric and string comparisons
Expected behavior: Outputs lines of 0/1 matching truth values.
*/
TEST(E2E, RelationalOps_Numeric_And_String) {
    if (!toolExists(CLANG_PATH)) {
        GTEST_SKIP() << "clang not found (CLANG_PATH='" << CLANG_PATH << "'), skipping E2E.";
    }
    const auto src =
        "10 PRINT 1 < 2\n"     // 1
        "20 PRINT 2 = 2\n"     // 1
        "30 PRINT 3 <= 2\n"    // 0
        "40 PRINT \"A\" < \"B\"\n" // 1
        "50 PRINT \"A\" = \"A\"\n" // 1
        "60 PRINT \"B\" <> \"A\"\n" // 1
        "70 END\n";

    std::string ir = Compiler::compileString(src);
    std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_relops";
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
        "1\n"
        "1\n"
        "0\n"
        "1\n"
        "1\n"
        "1\n";
    ASSERT_EQ(out, expected);
}

