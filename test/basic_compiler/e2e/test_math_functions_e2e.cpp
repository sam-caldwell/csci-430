// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include "basic_compiler/Compiler.h"
#include "clang_path.h"
#include "run_command.h"
#include "tool_exists.h"

using namespace gwbasic;
using namespace e2e_helpers;

TEST(E2E, MathFunctionsProduceExpectedResults) {
    if (!toolExists(CLANG_PATH)) {
        GTEST_SKIP() << "clang not found (CLANG_PATH='" << CLANG_PATH << "'), skipping E2E.";
    }
    // Print a range of function results, one per line
    const auto src =
        "10 PRINT SQR(9)\n"
        "20 PRINT ABS(-5)\n"
        "30 PRINT SIN(0)\n"
        "40 PRINT COS(0)\n"
        "50 PRINT TAN(0)\n"
        "60 PRINT ATN(1)\n"
        "70 PRINT LOG(1)\n"
        "80 PRINT EXP(1)\n"
        "90 PRINT INT(1.9)\n"
        "100 PRINT FIX(-1.9)\n"
        "110 PRINT SGN(-2)\n"
        "120 PRINT SGN(0)\n"
        "130 PRINT SGN(2)\n"
        "140 END\n";

    std::string ir = Compiler::compileString(src);

    std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_math";
    std::filesystem::create_directories(tmp);
    std::filesystem::path ll = tmp / "program.ll";
    std::filesystem::path bin = tmp / "program.out";
    { std::ofstream f(ll); f << ir; }

    std::ostringstream cmd; cmd << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
    int ec = std::system(cmd.str().c_str());
    ASSERT_EQ(ec, 0);

    std::ostringstream run; run << '"' << bin.string() << '"';
    std::string out = runCommand(run.str());

    // Compare expected values with 6 decimal places as emitted by %f
    const std::string expected =
        "3.000000\n"
        "5.000000\n"
        "0.000000\n"
        "1.000000\n"
        "0.000000\n"
        "0.785398\n" // atan(1)
        "0.000000\n" // log(1)
        "2.718282\n" // exp(1)
        "1.000000\n" // int(1.9)
        "-1.000000\n" // fix(-1.9)
        "-1.000000\n" // sgn(-2)
        "0.000000\n"  // sgn(0)
        "1.000000\n"; // sgn(2)
    ASSERT_EQ(out, expected);
}

