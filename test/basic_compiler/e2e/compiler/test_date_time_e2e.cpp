// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <regex>
#include <sstream>
#include <string>
#include "basic_compiler/Compiler.h"
#include "../../helper/clang_path.h"
#include "../../helper/run_command.h"
#include "../../helper/tool_exists.h"
#include "../../helper/split_lines.h"

using namespace gwbasic;
using namespace e2e_helpers;

/***
 * Test: E2E.DateTime_PrintsFormattedStrings
 * Purpose: Compile and run a program that prints DATE$ and TIME$. Validate
 *          that outputs match expected formats (mm-dd-yy and HH:MM:SS).
 */
TEST(E2E, DateTime_PrintsFormattedStrings) {
    if (!toolExists(CLANG_PATH)) {
        GTEST_SKIP() << "clang not found (CLANG_PATH='" << CLANG_PATH << "'), skipping E2E.";
    }
    const char* src =
        "10 PRINT DATE$\n"
        "20 PRINT TIME$\n";

    std::string ir = Compiler::compileString(src);
    ASSERT_FALSE(ir.empty());

    std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_date_time";
    std::filesystem::create_directories(tmp);
    auto ll = tmp / "program.ll";
    auto bin = tmp / "program.out";
    { std::ofstream f(ll); f << ir; }

    std::ostringstream c1; c1 << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    c1 << " -lm";
#endif
    int ec = std::system(c1.str().c_str());
    ASSERT_EQ(ec, 0);

    std::string out = runCommand(std::string("\"") + bin.string() + "\"");
    auto lines = splitLines(out);
    ASSERT_EQ(lines.size(), 2u);

    // DATE$: mm-dd-yy (two digits, hyphen-separated)
    std::regex re_date(R"(^\d{2}-\d{2}-\d{2}$)");
    EXPECT_TRUE(std::regex_match(lines[0], re_date)) << "DATE$ output not in mm-dd-yy: '" << lines[0] << "'";

    // TIME$: HH:MM:SS (24-hour, zero-padded)
    std::regex re_time(R"(^\d{2}:\d{2}:\d{2}$)");
    EXPECT_TRUE(std::regex_match(lines[1], re_time)) << "TIME$ output not in HH:MM:SS: '" << lines[1] << "'";
}

