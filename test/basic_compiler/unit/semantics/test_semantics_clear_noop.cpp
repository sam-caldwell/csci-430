// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
Test: Semantics.ClearNoop
Inputs: CLEAR
Code under test: SemanticAnalyzer::analyzeStmt(ClearStmt)
Expected behavior: Program compiles
*/
TEST(Semantics, ClearNoop) {
    const const std::string ir = Compiler::compileString("10 CLEAR\n");
    ASSERT_FALSE(ir.empty());
}
