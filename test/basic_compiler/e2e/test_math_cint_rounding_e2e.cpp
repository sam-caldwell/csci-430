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

TEST(E2E, CintRoundsHalfAwayFromZero) {
    if (!toolExists(CLANG_PATH)) {
        GTEST_SKIP() << "clang not found (CLANG_PATH='" << CLANG_PATH << "'), skipping E2E.";
    }
    const auto src =
        "10 PRINT CINT(1.5)\n"
        "20 PRINT CINT(-1.5)\n"
        "30 END\n";

    std::string ir = Compiler::compileString(src);

    std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_cint_round";
    std::filesystem::create_directories(tmp);
    std::filesystem::path ll = tmp / "program.ll";
    std::filesystem::path bin = tmp / "program.out";
    { std::ofstream f(ll); f << ir; }

    std::ostringstream cmd; cmd << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
    int ec = std::system(cmd.str().c_str());
    ASSERT_EQ(ec, 0);

    std::ostringstream run; run << '"' << bin.string() << '"';
    std::string out = runCommand(run.str());
    const std::string expected =
        "2.000000\n"
        "-2.000000\n";
    ASSERT_EQ(out, expected);
}

