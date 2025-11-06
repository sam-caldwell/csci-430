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
#include "../helper/split_lines.h"

using namespace gwbasic;
using namespace e2e_helpers;

/***
 * Test: E2E.ERASE_Redim_AllowsReuse
 * Purpose: Validate that ERASE followed by re-DIM compiles and runs.
 * Expected: Program runs without output or errors.
 */
/*
Test: E2E.ERASE_Redim_AllowsReuse
Inputs: Program DIMs arrays, ERASEs them, then re-DIMs; no prints
Code under test: Full pipeline
Expected behavior: Program compiles and runs producing no output
*/
TEST(E2E, ERASE_Redim_AllowsReuse) {
    if (!toolExists(CLANG_PATH)) { GTEST_SKIP() << "clang not found"; }
    const char* src =
        "10 DIM A(3)\n"
        "11 DIM S$(2)\n"
        "20 A(1)=5\n"
        "21 S$(1)=\"X\"\n"
        "30 ERASE A,S$\n"
        "35 DIM A(3)\n"
        "36 DIM S$(2)\n"
        "60 END\n";
    std::string ir = Compiler::compileString(src);
    std::filesystem::path tmp = std::filesystem::path("..")/"tmp"/"gwbasic_e2e_erase";
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
    ASSERT_EQ(lines.size(), 0u);
}
