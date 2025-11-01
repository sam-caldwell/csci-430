// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

TEST(SemanticsMathSqrt, SqrtNegativeConstantErrors) {
    const auto src =
        "10 PRINT SQRT(-1)\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}

TEST(SemanticsMathSqrt, SqrtComparisonArgumentErrors) {
    const auto src =
        "10 PRINT SQRT(1<2)\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}

