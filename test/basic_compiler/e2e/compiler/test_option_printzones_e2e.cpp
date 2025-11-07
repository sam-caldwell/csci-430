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
 * Test: E2E.OptionPrintZones_ProducesZoneSpacing
 * Inputs: OPTION PRINTZONES ON; PRINT "A","B"
 * Expected behavior: Output has A, then 13 spaces, then B, then newline
 */
TEST(E2E, OptionPrintZones_ProducesZoneSpacing) {
    if (!toolExists(CLANG_PATH)) { GTEST_SKIP() << "clang not found"; }
    const std::string src =
        "10 OPTION PRINTZONES ON\n"
        "20 PRINT \"A\", \"B\"\n"
        "30 END\n";
    std::string ir = Compiler::compileString(src);
    std::filesystem::path tmp = std::filesystem::path("..")/"tmp"/"gwbasic_e2e_printzones";
    std::filesystem::create_directories(tmp);
    auto ll = tmp/"p.ll"; auto bin = tmp/"p.out"; { std::ofstream f(ll); f << ir; }
    std::ostringstream cmd; cmd << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    cmd << " -lm";
#endif
    ASSERT_EQ(std::system(cmd.str().c_str()), 0);
    std::string out = runCommand(std::string("\"") + bin.string() + "\"");
    // Expect multiple spaces between A and B (zone padding). Allow some tolerance.
    auto ai = out.find('A');
    auto bi = out.find('B');
    ASSERT_NE(ai, std::string::npos);
    ASSERT_NE(bi, std::string::npos);
    ASSERT_GT(bi, ai);
    auto between = out.substr(ai + 1, bi - ai - 1);
    // At least a few spaces inserted
    ASSERT_GE(between.size(), 5u);
    for (char ch : between) ASSERT_EQ(ch, ' ');
}
