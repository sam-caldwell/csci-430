// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
Test: Semantics.MidAssignPositive
Inputs: MID$ assignment on scalar and string array element with numeric indices
Code under test: SemanticAnalyzer::analyzeStmt(MidAssignStmt)
Expected behavior: Programs compile (no semantic errors)
*/
TEST(Semantics, MidAssignPositive) {
    {
        const char* src =
            "10 A$=\"HELLO\"\n"
            "20 MID$(A$,2,1)=\"X\"\n";
        const const std::string ir = Compiler::compileString(src);
        ASSERT_FALSE(ir.empty());
    }
    {
        const char* src =
            "10 DIM A$(3)\n"
            "20 MID$(A$(1),1)=\"Z\"\n";
        const const std::string ir = Compiler::compileString(src);
        ASSERT_FALSE(ir.empty());
    }
}
