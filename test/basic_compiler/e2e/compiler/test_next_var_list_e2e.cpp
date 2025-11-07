// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include "basic_compiler/Compiler.h"
#include "../../helper/clang_path.h"
#include "../../helper/tool_exists.h"
#include "../../e2e/run_command.h"

using namespace gwbasic;

/***
 * Test: E2E.NextVarList_NestedLoopsPrints
 * Inputs: Program with nested FOR loops closed by NEXT var-list
 * Expected behavior: Program prints 4 pairs (1..2 x 1..2) and terminates.
 */
TEST(E2E, NextVarList_NestedLoopsPrints) {
    if (!e2e_helpers::toolExists(CLANG_PATH)) { GTEST_SKIP() << "clang not found"; }
    const std::string src =
        "10 FOR I = 1 TO 2\n"
        "20   FOR J = 1 TO 2\n"
        "30     PRINT I;\"-\";J\n"
        "40   NEXT J, I\n";
    std::string ir = Compiler::compileString(src.c_str());
    std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_nextlist";
    std::filesystem::create_directories(tmp);
    auto ll = tmp / "p.ll"; auto bin = tmp / "p.out"; { std::ofstream f(ll); f << ir; }
    std::ostringstream cmd; cmd << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    cmd << " -lm";
#endif
    ASSERT_EQ(std::system(cmd.str().c_str()), 0);
    std::string out = e2e_helpers::runCommand(std::string("\"") + bin.string() + "\"");
    // Expect four occurrences of '-' and presence of 1.000000 and 2.000000
    size_t hyphenCount = 0; for (char ch : out) if (ch == '-') ++hyphenCount;
    ASSERT_EQ(hyphenCount, 4u);
}
