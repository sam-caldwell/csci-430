// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

TEST(SemanticsMathDomains, SqrtNegativeConstantErrors) {
    const auto src =
        "10 PRINT SQR(-1)\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}

TEST(SemanticsMathDomains, LogZeroAndNegativeConstantErrors) {
    const auto src1 =
        "10 PRINT LOG(0)\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src1); (void)ir; }, SemanticError);
    const auto src2 =
        "10 PRINT LOG(-1)\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src2); (void)ir; }, SemanticError);
}

TEST(SemanticsMathDomains, SqrtNegativeNonConstantPasses) {
    const auto src =
        "10 LET A = -1\n"
        "20 PRINT SQR(A)\n"
        "30 END\n";
    // Domain unknown at semantic time for non-constant; should compile
    std::string ir = Compiler::compileString(src);
    EXPECT_NE(ir.find("call double @sqrt(double"), std::string::npos);
}

TEST(SemanticsMathDomains, LogOfComparisonErrors) {
    const auto src =
        "10 PRINT LOG(1<2)\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}

TEST(SemanticsMathDomains, SqrOfComparisonErrors) {
    const auto src =
        "10 PRINT SQR(2<3)\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}

TEST(SemanticsMathDomains, SqrtOfNegativeArithmeticConstantErrors) {
    const auto src =
        "10 PRINT SQR(-5+3)\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}

TEST(SemanticsMathDomains, LogOfZeroArithmeticConstantErrors) {
    const auto src =
        "10 PRINT LOG(1-1)\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}
