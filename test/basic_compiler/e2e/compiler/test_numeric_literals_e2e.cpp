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

using namespace gwbasic;
using namespace e2e_helpers;

/*
 * Test: E2E.NumericLiterals_OctalBinaryScientific
 * Inputs: BASIC program printing various numeric literal forms
 * Code under test: Full compiler pipeline
 * Expected behavior: Runtime prints expected decimal values.
 */
TEST(E2E, NumericLiterals_OctalBinaryScientific) {
    if (!toolExists(CLANG_PATH)) {
        GTEST_SKIP() << "clang not found (CLANG_PATH='" << CLANG_PATH << "'), skipping E2E.";
    }
    const auto src =
        "10 PRINT &H10\n"
        "20 PRINT &O10\n"
        "30 PRINT &777\n"
        "40 PRINT &B1010\n"
        "50 PRINT 1D2\n"
        "60 END\n";

    std::string ir = Compiler::compileString(src);
    std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_numlit";
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
        "16\n"
        "8\n"
        "511\n"
        "10\n"
        "100\n";
    ASSERT_EQ(out, expected);
}

