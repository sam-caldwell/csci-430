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
Test: E2E.Exponentiation_Precedence_And_Associativity
Inputs: BASIC program exercising '^' operator
Expected behavior: Correct results for simple power, right-associative chaining, and unary precedence.
*/
TEST(E2E, Exponentiation_Precedence_And_Associativity) {
    if (!toolExists(CLANG_PATH)) {
        GTEST_SKIP() << "clang not found (CLANG_PATH='" << CLANG_PATH << "'), skipping E2E.";
    }
    const auto src =
        "10 PRINT 2 ^ 3\n"      // 8
        "20 PRINT 2 ^ 3 ^ 2\n"  // 512 (right-associative)
        "30 PRINT -2 ^ 2\n"     // -4 (power binds tighter than unary minus)
        "40 PRINT (-2) ^ 2\n"   // 4
        "50 PRINT 2 ^ -3\n"     // 0.125
        "60 END\n";

    std::string ir = Compiler::compileString(src);
    std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_pow";
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
        "8\n"
        "512\n"
        "-4\n"
        "4\n"
        "0.125000\n";
    ASSERT_EQ(out, expected);
}
