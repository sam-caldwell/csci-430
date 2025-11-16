// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "basic_compiler/Compiler.h"
#include "../../helper/clang_path.h"
#include "../../helper/run_command.h"
#include "../../helper/tool_exists.h"
#include "../../helper/split_lines.h"

using namespace gwbasic;
using namespace e2e_helpers;

/***
 * Test: E2E.StringArray_MidAssign_Works
 * Purpose: Ensure MID$ assignment works on string array elements and prints the updated result.
 * Components Under Test: Parser/Codegen for MID$ with array element; End-to-End run.
 * Expected Behavior: Printed lines reflect the mid-assignment changes.
 */
TEST(E2E, StringArray_MidAssign_Works) {
    if (!toolExists(CLANG_PATH)) {
        GTEST_SKIP() << "clang not found (CLANG_PATH='" << CLANG_PATH << "'), skipping E2E.";
    }
    const auto src =
        "10 DIM A$(5)\n"
        "20 A$(2)=\"ABCDE\"\n"
        "30 MID$(A$(2),3)=\"XY\"\n"
        "40 PRINT A$(2)\n"
        "50 END\n";
    const std::string ir = Compiler::compileString(src);
    const std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_string_array_mid";
    std::filesystem::create_directories(tmp);
    const std::filesystem::path ll = tmp / "program.ll";
    const std::filesystem::path bin = tmp / "program.out";
    { std::ofstream f(ll); f << ir; }
    std::ostringstream cmd; cmd << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    cmd << " -lm";
#endif
    const int ec = std::system(cmd.str().c_str());
    ASSERT_EQ(ec, 0);
    std::ostringstream run; run << '"' << bin.string() << '"';
    const std::string out = runCommand(run.str());
    auto lines = splitLines(out);
    ASSERT_EQ(lines.size(), 1u);
    EXPECT_EQ(lines[0], std::string("ABXYE"));
}
