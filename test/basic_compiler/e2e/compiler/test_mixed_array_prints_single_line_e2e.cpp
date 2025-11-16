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
 * Test Suite: E2E Mixed-Type Single-Line PRINT
 * Purpose: Mix scalar vars and arrays in one PRINT list; verify truncation and float formatting.
 */
TEST(E2E, MixedArrayPrints_SingleLine) {
    if (!toolExists(CLANG_PATH)) GTEST_SKIP();
    const char* src =
        "10 DIM E%(3)\n"
        "11 DIM F&(3)\n"
        "12 DIM G!(3)\n"
        "13 DIM H#(3)\n"
        "20 A%=1.9\n"
        "21 B&=-2.8\n"
        "22 C!=2.25\n"
        "23 D#=5.125\n"
        "30 E%(1)=3.1\n"
        "31 F&(1)=-7.9\n"
        "32 G!(1)=4.5\n"
        "33 H#(1)=6.75\n"
        "40 PRINT A%,E%(1),C!,G!(1),D#,H#(1),B&,F&(1)\n"
        "50 END\n";
    const std::string ir = Compiler::compileString(src);
    const std::filesystem::path tmp = std::filesystem::path("..")/"tmp"/"gwbasic_e2e_mixed_prints_single";
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
    ASSERT_EQ(lines.size(), 1u);
    EXPECT_EQ(lines[0], std::string("1 3 2.250000 4.500000 5.125000 6.750000 -2 -7"));
}
