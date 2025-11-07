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

/***
 * Test: E2E.PrintDoubledQuotes_Works
 * Inputs: BASIC program with a string literal that contains doubled quotes.
 * Code under test: Full pipeline + runtime output
 * Expected behavior: Prints a line with an embedded double quote.
 */
TEST(E2E, PrintDoubledQuotes_Works) {
    if (!toolExists(CLANG_PATH)) {
        GTEST_SKIP() << "clang not found (CLANG_PATH='" << CLANG_PATH << "'), skipping E2E.";
    }
    // BASIC: PRINT "He said ""OK"""
    const std::string src = "10 PRINT \"He said \"\"OK\"\"\"\n20 END\n";
    std::string ir = Compiler::compileString(src);

    std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e";
    std::filesystem::create_directories(tmp);
    std::filesystem::path ll = tmp / "print_dq.ll";
    std::filesystem::path bin = tmp / "print_dq.out";
    { std::ofstream f(ll); f << ir; }

    std::ostringstream c1; c1 << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    c1 << " -lm";
#endif
    int ec = std::system(c1.str().c_str());
    ASSERT_EQ(ec, 0) << "Clang failed: " << c1.str();

    std::ostringstream r1; r1 << '"' << bin.string() << '"';
    std::string out = runCommand(r1.str());
    ASSERT_NE(out.find("He said \"OK\"\n"), std::string::npos);
}
