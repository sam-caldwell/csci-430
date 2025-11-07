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
#include "source_root.h"

using namespace gwbasic;
using namespace e2e_helpers;

/*
 * Test: E2E.ConwaysLife_BuildsRunsAndPrints
 * Purpose: Compile and run the Conway's Life demo and validate that it
 *          starts, renders the initial frame, and prints the status line.
 * Components Under Test: Compiler (compileFile), clang driver, runtime output
 * Expected Behavior: Program compiles and runs; captured output contains
 *                    the rules/status text and the T= prefix.
 * Notes:
 *  - This demo is a simplified text-mode output. We compile and run it and
 *    validate key substrings without requiring interactive input.
 */
/*
Test: E2E.ConwaysLife_BuildsRunsAndPrints
Inputs: demos/conways.bas compiled end-to-end
Code under test: Full compiler pipeline (lexer → parser → semantics → codegen → runtime)
Expected behavior: Program compiles and runs; output contains status text
*/
TEST(E2E, ConwaysLife_BuildsRunsAndPrints) {
    if (!toolExists(CLANG_PATH)) {
        GTEST_SKIP() << "clang not found (CLANG_PATH='" << CLANG_PATH << "'), skipping E2E.";
    }

    const std::string demo = e2e_helpers::sourceRoot() + "/demos/conways.bas";

    // Compile demo from file
    std::string ir = Compiler::compileFile(demo.c_str());
    ASSERT_FALSE(ir.empty());

    // Write IR and compile with clang
    const std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_conways_life";
    std::filesystem::create_directories(tmp);
    const auto ll = tmp / "program.ll";
    const auto bin = tmp / "program.out";
    { std::ofstream f(ll); f << ir; }
    std::ostringstream c1; c1 << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    c1 << " -lm";
#endif
    const int ec = std::system(c1.str().c_str());
    ASSERT_EQ(ec, 0) << "Clang failed to build Conway's Life demo";

    // Run and capture output
    const std::string out = runCommand(std::string("\"") + bin.string() + "\"");

    // Basic sanity: we should have captured some output
    ASSERT_FALSE(out.empty());

    // Validate the status line content appears
    ASSERT_NE(out.find("A eats C, B eats D, D eats B, C eats A"), std::string::npos)
        << "Expected rules/status text not found in output.";
    // Validate the T= prefix (time/iteration counter) is present
    ASSERT_NE(out.find("T="), std::string::npos) << "Expected 'T=' not found in output.";
}
