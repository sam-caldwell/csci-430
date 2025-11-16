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
#include "../../helper/split_lines.h"

using namespace gwbasic;
using namespace e2e_helpers;

/***
 * Test: E2E.SWAP_Numeric_And_String
 * Inputs: Program swaps X and Y; swaps A$ and B$; prints results
 * Code under test: Full pipeline
 * Expected behavior: Output has lines "2 1" and then "bar" and "foo"
 */
TEST(E2E, SWAP_Numeric_And_String) {
    if (!toolExists(CLANG_PATH)) { GTEST_SKIP() << "clang not found"; }
    const char* src =
        "10 X=1\n"
        "11 Y=2\n"
        "12 SWAP X,Y\n"
        "13 PRINT X,Y\n"
        "20 A$=\"foo\"\n"
        "21 B$=\"bar\"\n"
        "22 SWAP A$,B$\n"
        "23 PRINT A$\n"
        "24 PRINT B$\n"
        "30 END\n";
    const std::string ir = Compiler::compileString(src);
    const std::filesystem::path tmp = std::filesystem::path("..")/"tmp"/"gwbasic_e2e_swap";
    std::filesystem::create_directories(tmp);
    auto ll = tmp/"program.ll"; auto bin = tmp/"program.out";
    { std::ofstream f(ll); f << ir; }
    std::ostringstream cmd; cmd << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    cmd << " -lm";
#endif
    ASSERT_EQ(std::system(cmd.str().c_str()), 0);
    const std::string out = runCommand(std::string("\"") + bin.string() + "\"");
    auto lines = splitLines(out);
    ASSERT_FALSE(lines.empty());
    // First print line should contain "2 1"
    bool sawNums = false;
    for (const auto& ln : lines) { if (ln.find("2") != std::string::npos && ln.find("1") != std::string::npos) { sawNums = true; break; } }
    ASSERT_TRUE(sawNums);
    // Later lines should contain swapped strings
    bool sawBar = false, sawFoo = false;
    for (const auto& ln : lines) { if (ln.find("bar") != std::string::npos) sawBar = true; if (ln.find("foo") != std::string::npos) sawFoo = true; }
    ASSERT_TRUE(sawBar);
    ASSERT_TRUE(sawFoo);
}
