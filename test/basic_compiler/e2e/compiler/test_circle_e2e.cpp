// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include "basic_compiler/Compiler.h"
#include "clang_path.h"
#include "run_command.h"
#include "../../helper/tool_exists.h"

using namespace gwbasic;
using namespace e2e_helpers;

/***
 * Test: E2E.CIRCLE_CompilesAndRunsSafely
 * Purpose: Validate end-to-end behavior: CIRCLE compiles and runs without crash;
 *          program remains safe on headless systems (no graphics window).
 * Expected: Runtime exits 0 and prints expected line.
 */
/*
Test: E2E.CIRCLE_CompilesAndRunsSafely
Inputs: Program with SCREEN 1, CIRCLE ... and PRINT/END
Code under test: Full pipeline including graphics guard
Expected behavior: Binary runs and prints "CIRCLE OK" even if no display is available
*/
TEST(E2E, CIRCLE_CompilesAndRunsSafely) {
    if (!toolExists(CLANG_PATH)) { GTEST_SKIP() << "clang not found"; }
    std::string src =
        "10 SCREEN 1\n"
        "20 CIRCLE (100,100), 50, 3, 0, 6.28, 1.0\n"
        "30 PRINT \"CIRCLE OK\"\n"
        "40 END\n";
    std::string ir = Compiler::compileString(src);
    std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_circle";
    std::filesystem::create_directories(tmp);
    std::filesystem::path ll = tmp / "program.ll";
    std::filesystem::path bin = tmp / "program.out";
    { std::ofstream f(ll); f << ir; }
    std::ostringstream c2; c2 << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    c2 << " -lm";
#endif
    int ec = std::system(c2.str().c_str());
    ASSERT_EQ(ec, 0);
    std::string out = runCommand(std::string("\"") + bin.string() + "\"");
    ASSERT_NE(out.find("CIRCLE OK\n"), std::string::npos);
}

