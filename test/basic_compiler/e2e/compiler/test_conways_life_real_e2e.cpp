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
Test: E2E.ConwaysLife_RealDemo_BuildsRunsAndPrints
Inputs: demos/conways-life.bas compiled end-to-end
Code under test: Full compiler pipeline (lexer → parser → semantics → codegen → runtime)
Expected behavior: Program compiles and runs; captured output contains rules/status text and T=
Notes:
- The program runs an infinite loop until a key is pressed via INKEY$. We run
  the binary through `head -c` to capture the initial output and terminate.
*/
TEST(E2E, ConwaysLife_RealDemo_BuildsRunsAndPrints) {
    if (!toolExists(CLANG_PATH)) {
        GTEST_SKIP() << "clang not found (CLANG_PATH='" << CLANG_PATH << "'), skipping E2E.";
    }
    const std::string demo = e2e_helpers::sourceRoot() + "/demos/conways-life.bas";
    std::string ir = Compiler::compileFile(demo.c_str());
    ASSERT_FALSE(ir.empty());

    const std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_conway_real";
    std::filesystem::create_directories(tmp);
    const auto ll = tmp / "program.ll";
    const auto bin = tmp / "program.out";
    { std::ofstream f(ll); f << ir; }
    std::ostringstream c1; c1 << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    c1 << " -lm";
#endif
    ASSERT_EQ(std::system(c1.str().c_str()), 0);

    // Guarded run: capture first 32 KiB to avoid infinite loop
    std::ostringstream rc; rc << "sh -c '\"" << bin.string() << "\" | head -c 32768'";
    const std::string out = runCommand(rc.str());
    ASSERT_FALSE(out.empty());
    ASSERT_NE(out.find("A eats C, B eats D, D eats B, C eats A"), std::string::npos);
    ASSERT_NE(out.find("T="), std::string::npos);
}

