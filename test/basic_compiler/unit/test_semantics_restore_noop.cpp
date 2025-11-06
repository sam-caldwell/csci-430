// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
Test: Semantics.RestoreNoop
Inputs: RESTORE without arguments
Code under test: SemanticAnalyzer::analyzeStmt(RestoreStmt)
Expected behavior: Program compiles
*/
TEST(Semantics, RestoreNoop) {
    const std::string ir = Compiler::compileString("10 RESTORE\n");
    ASSERT_FALSE(ir.empty());
}

