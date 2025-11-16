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
 * Test: E2E.MidAssign_Behavior
 * Purpose: Validate MID$ behavior with and without length over multiple cases.
 * Components Under Test: Parser/Codegen for MID$ assignment; End-to-End run.
 * Expected Behavior: Output matches the three expected transformed strings.
 */
TEST(E2E, MidAssign_Behavior) {
    if (!toolExists(CLANG_PATH)) {
        GTEST_SKIP() << "clang not found (CLANG_PATH='" << CLANG_PATH << "'), skipping E2E.";
    }
    const auto src =
        // No len: copies length of RHS, truncated by dest end
        "10 S$=\"ABCDE\"\n"
        "20 MID$(S$,3)=\"xyz\"\n"      // ABxyz
        // Len shorter than RHS: truncate source to len
        "30 T$=\"ABCDE\"\n"
        "40 MID$(T$,2,2)=\"ZZZ\"\n"   // AZZDE
        // Len longer than RHS: only replace available bytes, remainder unchanged
        "50 U$=\"HELLO\"\n"
        "60 MID$(U$,5,3)=\"XY\"\n"    // HELLX
        "70 PRINT S$\n"
        "80 PRINT T$\n"
        "90 PRINT U$\n"
        "100 END\n";
    const std::string ir = Compiler::compileString(src);
    const std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_mid_assign";
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
    ASSERT_EQ(lines.size(), 3u);
    EXPECT_EQ(lines[0], std::string("ABxyz"));
    EXPECT_EQ(lines[1], std::string("AZZDE"));
    EXPECT_EQ(lines[2], std::string("HELLX"));
}
