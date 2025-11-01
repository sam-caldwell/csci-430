// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/*
 * Test Suite: Semantics Type Errors
 * Purpose: Cover sad-path diagnostics in the semantic analyzer.
 */
TEST(Semantics, ArithmeticOnStringErrors) {
    const auto src =
        "10 LET A = \"A\" + 1\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}

TEST(Semantics, UnaryMinusOnStringErrors) {
    const auto src =
        "10 LET A = -\"X\"\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}

TEST(Semantics, MixedStringNumberComparisonErrors) {
    const auto src =
        "10 IF \"A\" = 1 THEN 20\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}

TEST(Semantics, IfConditionStringErrors) {
    const auto src =
        "10 IF \"A\" THEN 20\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}
