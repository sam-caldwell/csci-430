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
Test: E2E.LenValInstr_ProduceExpectedResults
Inputs: Program that prints LEN("HELLO"), INSTR("HELLO","EL"), INSTR(3,"HELLO","L"), VAL("42")
Code under test: End-to-end compile/link/run
Expected behavior: Output lines: 5, 2, 3, 42
*/
TEST(E2E, LenValInstr_ProduceExpectedResults) {
    if (!toolExists(CLANG_PATH)) {
        GTEST_SKIP() << "clang not found (CLANG_PATH='" << CLANG_PATH << "'), skipping E2E.";
    }
    const auto src =
        "10 PRINT LEN(\"HELLO\")\n"
        "20 PRINT INSTR(\"HELLO\",\"EL\")\n"
        "30 PRINT INSTR(3,\"HELLO\",\"L\")\n"
        "40 PRINT VAL(\"42\")\n"
        "50 END\n";

    std::string ir = Compiler::compileString(src);
    std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_len_val_instr";
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
    auto lines = splitLines(out);
    ASSERT_EQ(lines.size(), 4u);
    EXPECT_EQ(lines[0], std::string("5"));
    EXPECT_EQ(lines[1], std::string("2"));
    EXPECT_EQ(lines[2], std::string("3"));
    EXPECT_EQ(lines[3], std::string("42"));
}

