// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include "basic_compiler/semantics/SemanticAnalyzer.h"
#include "basic_compiler/ast/NumberExpr.h"
#include "basic_compiler/ast/UnaryExpr.h"
#include "basic_compiler/ast/BinaryExpr.h"
#include "basic_compiler/ast/BinaryOp.h"

using namespace gwbasic;

/***
Test: SemanticsConstEval.BroadArithmeticAndComparisons
Purpose: Cover constant evaluation across arithmetic and comparison operators.
Components Under Test: SemanticAnalyzer::constEval via test accessor.
Expected Behavior: Returns true and produces expected numeric results.
*/
TEST(SemanticsConstEval, BroadArithmeticAndComparisons) {
    double out = 0.0;
    // 1+2
    auto e12 = std::make_unique<BinaryExpr>(BinaryOp::Add,
                                            std::make_unique<NumberExpr>(1.0),
                                            std::make_unique<NumberExpr>(2.0));
    ASSERT_TRUE(SemanticAnalyzerConstEvalAccessorForTests::constEval(e12.get(), out));
    EXPECT_DOUBLE_EQ(out, 3.0);
    // 4-3
    auto e43 = std::make_unique<BinaryExpr>(BinaryOp::Sub,
                                            std::make_unique<NumberExpr>(4.0),
                                            std::make_unique<NumberExpr>(3.0));
    ASSERT_TRUE(SemanticAnalyzerConstEvalAccessorForTests::constEval(e43.get(), out));
    EXPECT_DOUBLE_EQ(out, 1.0);
    // 2*3
    auto e23 = std::make_unique<BinaryExpr>(BinaryOp::Mul,
                                            std::make_unique<NumberExpr>(2.0),
                                            std::make_unique<NumberExpr>(3.0));
    ASSERT_TRUE(SemanticAnalyzerConstEvalAccessorForTests::constEval(e23.get(), out));
    EXPECT_DOUBLE_EQ(out, 6.0);
    // 8/4
    auto e84 = std::make_unique<BinaryExpr>(BinaryOp::Div,
                                            std::make_unique<NumberExpr>(8.0),
                                            std::make_unique<NumberExpr>(4.0));
    ASSERT_TRUE(SemanticAnalyzerConstEvalAccessorForTests::constEval(e84.get(), out));
    EXPECT_DOUBLE_EQ(out, 2.0);
    // unary minus
    auto u2 = std::make_unique<UnaryExpr>('-', std::make_unique<NumberExpr>(2.0));
    ASSERT_TRUE(SemanticAnalyzerConstEvalAccessorForTests::constEval(u2.get(), out));
    EXPECT_DOUBLE_EQ(out, -2.0);
    // comparisons
    auto c1 = std::make_unique<BinaryExpr>(BinaryOp::Eq,
                                           std::make_unique<NumberExpr>(1.0),
                                           std::make_unique<NumberExpr>(1.0));
    ASSERT_TRUE(SemanticAnalyzerConstEvalAccessorForTests::constEval(c1.get(), out));
    EXPECT_DOUBLE_EQ(out, 1.0);
    auto c2 = std::make_unique<BinaryExpr>(BinaryOp::Ne,
                                           std::make_unique<NumberExpr>(2.0),
                                           std::make_unique<NumberExpr>(1.0));
    ASSERT_TRUE(SemanticAnalyzerConstEvalAccessorForTests::constEval(c2.get(), out));
    EXPECT_DOUBLE_EQ(out, 1.0);
    auto c3 = std::make_unique<BinaryExpr>(BinaryOp::Lt,
                                           std::make_unique<NumberExpr>(1.0),
                                           std::make_unique<NumberExpr>(2.0));
    ASSERT_TRUE(SemanticAnalyzerConstEvalAccessorForTests::constEval(c3.get(), out));
    EXPECT_DOUBLE_EQ(out, 1.0);
    auto c4 = std::make_unique<BinaryExpr>(BinaryOp::Le,
                                           std::make_unique<NumberExpr>(2.0),
                                           std::make_unique<NumberExpr>(2.0));
    ASSERT_TRUE(SemanticAnalyzerConstEvalAccessorForTests::constEval(c4.get(), out));
    EXPECT_DOUBLE_EQ(out, 1.0);
    auto c5 = std::make_unique<BinaryExpr>(BinaryOp::Gt,
                                           std::make_unique<NumberExpr>(3.0),
                                           std::make_unique<NumberExpr>(2.0));
    ASSERT_TRUE(SemanticAnalyzerConstEvalAccessorForTests::constEval(c5.get(), out));
    EXPECT_DOUBLE_EQ(out, 1.0);
    auto c6 = std::make_unique<BinaryExpr>(BinaryOp::Ge,
                                           std::make_unique<NumberExpr>(3.0),
                                           std::make_unique<NumberExpr>(3.0));
    ASSERT_TRUE(SemanticAnalyzerConstEvalAccessorForTests::constEval(c6.get(), out));
    EXPECT_DOUBLE_EQ(out, 1.0);
}
