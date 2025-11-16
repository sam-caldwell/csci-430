// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
Test: Semantics.MiscPositive
Inputs: DEF USR, CHDIR, COLOR, SCREEN with valid argument types
Code under test: SemanticAnalyzer::analyzeStmt for these statements
Expected behavior: Programs compile (no semantic errors)
*/
TEST(Semantics, MiscPositive) {
    const char* src =
        "10 DEF USR0=0\n"
        "20 CHDIR \"/tmp\"\n"
        "30 COLOR 1, 2, 3\n"
        "40 SCREEN 0, 1, 0, 0\n"
        "50 END\n";
    const const std::string ir = Compiler::compileString(src);
    ASSERT_FALSE(ir.empty());
}
