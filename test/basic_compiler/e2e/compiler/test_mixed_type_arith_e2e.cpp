// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include "basic_compiler/Compiler.h"
#include "clang_path.h"
#include "run_command.h"
#include "../../helper/tool_exists.h"
#include "../../helper/split_lines.h"

using namespace gwbasic;
using namespace e2e_helpers;

/*
Test: E2E.MixedTypeArithmetic_ComputeInDouble_StoreByType
Inputs: Program mixing Int16/Long/Single/Double in expressions and assignments
Code under test: End-to-end compile/link/run
Expected behavior: Arithmetic computes in double; stores truncate or round per target type; outputs match
*/
TEST(E2E, MixedTypeArithmetic_ComputeInDouble_StoreByType) {
    if (!toolExists(CLANG_PATH)) {
        GTEST_SKIP() << "clang not found (CLANG_PATH='" << CLANG_PATH << "'), skipping E2E.";
    }
    const char* src =
        // Truncation on integer stores
        "10 A% = 1.9\n"
        "20 B& = -1.9\n"
        // Promotion from int to double in division
        "30 C# = A% / 2\n"
        "40 D& = 3\n"
        "50 E# = D& / 2\n"
        // Mixed single + double add computes in double
        "60 F! = 0.2\n"
        "70 G# = F! + 0.4\n"
        "80 PRINT A%\n"
        "90 PRINT B&\n"
        "100 PRINT C#\n"
        "110 PRINT E#\n"
        "120 PRINT G#\n"
        "130 END\n";

    std::string ir = Compiler::compileString(src);
    std::filesystem::path tmp = std::filesystem::path("..")/"tmp"/"gwbasic_e2e_mixed_type_arith";
    std::filesystem::create_directories(tmp);
    auto ll = tmp/"program.ll";
    auto bin = tmp/"program.out";
    { std::ofstream f(ll); f << ir; }

    std::ostringstream cmd; cmd << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    cmd << " -lm";
#endif
    ASSERT_EQ(std::system(cmd.str().c_str()), 0);

    std::string out = runCommand(std::string("\"") + bin.string() + "\"");
    auto lines = splitLines(out);
    ASSERT_EQ(lines.size(), 5u);
    EXPECT_EQ(lines[0], std::string("1"));          // 1.9 -> 1 (trunc)
    EXPECT_EQ(lines[1], std::string("-1"));         // -1.9 -> -1 (trunc toward zero)
    EXPECT_EQ(lines[2], std::string("0.500000"));   // A% (1) / 2 => 0.5
    EXPECT_EQ(lines[3], std::string("1.500000"));   // D& (3) / 2 => 1.5
    EXPECT_EQ(lines[4], std::string("0.600000"));   // 0.2 (single) + 0.4 (double) => 0.6
}

