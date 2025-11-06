// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
Test: Semantics.ChainMinimal
Inputs: CHAIN with only a filename (no target line or ALL)
Code under test: SemanticAnalyzer::analyzeStmt(ChainStmt)
Expected behavior: Program compiles; semantics logs Chain
*/
TEST(Semantics, ChainMinimal) {
    const char* src =
        "10 CHAIN \"PROG\"\n"
        "20 END\n";
    const std::string ir = Compiler::compileString(src);
    ASSERT_FALSE(ir.empty());
}

