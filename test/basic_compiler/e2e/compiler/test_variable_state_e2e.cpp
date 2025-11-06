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

/***
 * Test: E2E.VariableState_InitialValues
 * Purpose: Verify compiled program stores and prints initial variable values.
 * Expected (currently failing): "initial state: X= 99.000000 Y= 42.000000"
 */
/*
Test: E2E.VariableState_InitialValues
Inputs: demos/variable-state-test.bas
Code under test: Full pipeline (lexer → parser → semantics → codegen → runtime)
Expected behavior: Program should print initial state with X=99, Y=42 (as doubles)
*/
TEST(E2E, VariableState_InitialValues) {
    if (!toolExists(CLANG_PATH)) { GTEST_SKIP() << "clang not found"; }
    std::string srcIR = Compiler::compileFile((e2e_helpers::sourceRoot()+"/demos/variable-state-test.bas").c_str());
    std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_var_state";
    std::filesystem::create_directories(tmp);
    auto ll = tmp / "prog.ll"; auto bin = tmp / "prog.out";
    { std::ofstream f(ll); f << srcIR; }
    std::ostringstream cmd; cmd << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    cmd << " -lm";
#endif
    ASSERT_EQ(std::system(cmd.str().c_str()), 0);
    std::string out = runCommand(std::string("\"") + bin.string() + "\"");
    // Expect dynamic formatting: integers print without decimals
    ASSERT_NE(out.find("initial state: X= 99 Y= 42"), std::string::npos);
}
