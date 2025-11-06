// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/*
Test: Semantics.DefFnValidation
Inputs: Duplicate DEF; return type mismatches for DEF FN
Code under test: SemanticAnalyzer::analyzeStmt(DefFnStmt)
Expected behavior: SemanticError thrown for duplicates and mismatched return types
*/
TEST(Semantics, DefFnValidation) {
    // Duplicate DEF
    EXPECT_THROW({ auto ir = Compiler::compileString(
        "10 DEF FNA(A)=A\n"
        "20 DEF FNA(B)=B\n"); (void)ir; }, SemanticError);

    // String function must return string
    EXPECT_THROW({ auto ir = Compiler::compileString(
        "10 DEF FNS$(A$)=1\n"); (void)ir; }, SemanticError);

    // Numeric function must return number
    EXPECT_THROW({ auto ir = Compiler::compileString(
        "10 DEF FNN(A)=\"X\"\n"); (void)ir; }, SemanticError);
}

