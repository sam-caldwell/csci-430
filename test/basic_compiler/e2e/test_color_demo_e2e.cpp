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
#include "source_root.h"

using namespace gwbasic;
using namespace e2e_helpers;

/*
 * Test: E2E.ColorDemo_PrintsExpectedAnsiAndText
 * Purpose: Compile and run demos/working-with-color.bas and verify ANSI SGR
 *          sequences and printed lines appear as expected.
 * Components Under Test: Compiler (compileFile), clang driver, runtime output
 * Expected Behavior: Output contains color codes and corresponding text lines.
 */
/*
Test: E2E.ColorDemo_PrintsExpectedAnsiAndText
Inputs: BASIC program(s) executed end-to-end (runtime output)
Code under test: Full compiler pipeline (lexer → parser → semantics → codegen → runtime)
Expected behavior: Program compiles and runs; output/behavior matches expectations
*/
TEST(E2E, ColorDemo_PrintsExpectedAnsiAndText) {
    if (!toolExists(CLANG_PATH)) {
        GTEST_SKIP() << "clang not found (CLANG_PATH='" << CLANG_PATH << "'), skipping E2E.";
    }
    // Compile demo from file
    std::string ir = Compiler::compileFile((e2e_helpers::sourceRoot()+"/demos/working-with-color.bas").c_str());
    ASSERT_FALSE(ir.empty());

    // Write IR and compile with clang
    const std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_color_demo";
    std::filesystem::create_directories(tmp);
    const auto ll = tmp / "program.ll";
    const auto bin = tmp / "program.out";
    { std::ofstream f(ll); f << ir; }
    std::ostringstream c1; c1 << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    c1 << " -lm";
#endif
    const int ec = std::system(c1.str().c_str());
    ASSERT_EQ(ec, 0);

    // Run and capture output
    const std::string out = runCommand(std::string("\"") + bin.string() + "\"");

    // Validate text lines
    ASSERT_NE(out.find("Working with COLOR\n"), std::string::npos);
    ASSERT_NE(out.find("This is green on black\n"), std::string::npos);
    ASSERT_NE(out.find("This is red on black\n"), std::string::npos);
    ASSERT_NE(out.find("This is yellow on red\n"), std::string::npos);
    ASSERT_NE(out.find("Back to gray on black\n"), std::string::npos);

    // Validate ANSI SGR codes for each color change
    // COLOR 2,0 -> ESC[32m (green fg), ESC[40m (black bg)
    ASSERT_NE(out.find("\033[32m"), std::string::npos);
    ASSERT_NE(out.find("\033[40m"), std::string::npos);
    // COLOR 4,0 -> ESC[31m (red fg), ESC[40m (black bg)
    ASSERT_NE(out.find("\033[31m"), std::string::npos);
    // COLOR 14,4 -> ESC[93m (yellow fg), ESC[41m (red bg)
    ASSERT_NE(out.find("\033[93m"), std::string::npos);
    ASSERT_NE(out.find("\033[41m"), std::string::npos);
	// COLOR 7,0 -> ESC[37m (gray fg), ESC[40m (black bg)
	ASSERT_NE(out.find("\033[37m"), std::string::npos);

	// Validate SCREEN demo content
	ASSERT_NE(out.find("SCREEN function demo\n"), std::string::npos);
	// After printing "ABC" on row 7, SCREEN(7,1..3) should produce A,B,C ASCII codes
	ASSERT_NE(out.find("65"), std::string::npos);
	ASSERT_NE(out.find("66"), std::string::npos);
	ASSERT_NE(out.find("67"), std::string::npos);
}
