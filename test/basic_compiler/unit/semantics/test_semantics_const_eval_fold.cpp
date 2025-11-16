// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
Test: Semantics.ConstEval_FoldsArithmeticAndComparisons
Inputs: One line with various constant expressions (arithmetic, unary, comparisons)
Code under test: SemanticAnalyzer::constEval via analyzeExpr
Expected behavior: Compilation succeeds (const folding applies along the way).
*/
TEST(Semantics, ConstEval_FoldsArithmeticAndComparisons) {
    const char* src =
        "10 A=1+2 : B=3-4 : C=5*6 : D=8/2 : E=1=1 : F=2<3 : G=4<=4 : H=5>4 : I=5>=6 : J=-1 : K=+2\n";
    const std::string ir = Compiler::compileString(src);
    ASSERT_FALSE(ir.empty());
}

