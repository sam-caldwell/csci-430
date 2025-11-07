// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <memory>
#include "basic_compiler/semantics/SemanticAnalyzer.h"
#include "basic_compiler/ast/NumberExpr.h"
#include "basic_compiler/ast/UnaryExpr.h"
#include "basic_compiler/ast/BinaryExpr.h"
#include "basic_compiler/ast/BinaryOp.h"
#include "basic_compiler/Symbols.h"

using namespace gwbasic;

/***
Test: Semantics.ConstEvalBasicAndOps
Inputs: Manually constructed AST expressions
Code under test: SemanticAnalyzer::constEval via friend accessor
Expected behavior: Returns true with correct numeric results for literals,
                   unary +/- and arithmetic/comparison binary ops; returns
                   false for unsupported ops.
*/
TEST(Semantics, ConstEvalBasicAndOps) {
    double out = 0.0;

    // Literal
    {
        auto n = std::make_unique<NumberExpr>(7.0);
        ASSERT_TRUE(SemanticAnalyzerConstEvalAccessorForTests::constEval(n.get(), out));
        EXPECT_DOUBLE_EQ(out, 7.0);
    }
    // Unary + and -
    {
        auto up = std::make_unique<UnaryExpr>(Symbols::PLUS.first(), std::make_unique<NumberExpr>(2.5));
        ASSERT_TRUE(SemanticAnalyzerConstEvalAccessorForTests::constEval(up.get(), out));
        EXPECT_DOUBLE_EQ(out, 2.5);

        auto um = std::make_unique<UnaryExpr>(Symbols::MINUS.first(), std::make_unique<NumberExpr>(2.5));
        ASSERT_TRUE(SemanticAnalyzerConstEvalAccessorForTests::constEval(um.get(), out));
        EXPECT_DOUBLE_EQ(out, -2.5);
    }
    // Binary arithmetic
    {
        auto add = std::make_unique<BinaryExpr>(BinaryOp::Add, std::make_unique<NumberExpr>(1.0), std::make_unique<NumberExpr>(2.0));
        ASSERT_TRUE(SemanticAnalyzerConstEvalAccessorForTests::constEval(add.get(), out));
        EXPECT_DOUBLE_EQ(out, 3.0);

        auto sub = std::make_unique<BinaryExpr>(BinaryOp::Sub, std::make_unique<NumberExpr>(5.0), std::make_unique<NumberExpr>(3.0));
        ASSERT_TRUE(SemanticAnalyzerConstEvalAccessorForTests::constEval(sub.get(), out));
        EXPECT_DOUBLE_EQ(out, 2.0);

        auto mul = std::make_unique<BinaryExpr>(BinaryOp::Mul, std::make_unique<NumberExpr>(4.0), std::make_unique<NumberExpr>(2.5));
        ASSERT_TRUE(SemanticAnalyzerConstEvalAccessorForTests::constEval(mul.get(), out));
        EXPECT_DOUBLE_EQ(out, 10.0);

        auto div = std::make_unique<BinaryExpr>(BinaryOp::Div, std::make_unique<NumberExpr>(9.0), std::make_unique<NumberExpr>(4.5));
        ASSERT_TRUE(SemanticAnalyzerConstEvalAccessorForTests::constEval(div.get(), out));
        EXPECT_DOUBLE_EQ(out, 2.0);
    }
    // Binary comparisons
    {
        auto eq  = std::make_unique<BinaryExpr>(BinaryOp::Eq, std::make_unique<NumberExpr>(2.0), std::make_unique<NumberExpr>(2.0));
        ASSERT_TRUE(SemanticAnalyzerConstEvalAccessorForTests::constEval(eq.get(), out));
        EXPECT_DOUBLE_EQ(out, 1.0);
        auto ne  = std::make_unique<BinaryExpr>(BinaryOp::Ne, std::make_unique<NumberExpr>(2.0), std::make_unique<NumberExpr>(3.0));
        ASSERT_TRUE(SemanticAnalyzerConstEvalAccessorForTests::constEval(ne.get(), out));
        EXPECT_DOUBLE_EQ(out, 1.0);
        auto lt  = std::make_unique<BinaryExpr>(BinaryOp::Lt, std::make_unique<NumberExpr>(1.0), std::make_unique<NumberExpr>(2.0));
        ASSERT_TRUE(SemanticAnalyzerConstEvalAccessorForTests::constEval(lt.get(), out));
        EXPECT_DOUBLE_EQ(out, 1.0);
        auto le  = std::make_unique<BinaryExpr>(BinaryOp::Le, std::make_unique<NumberExpr>(2.0), std::make_unique<NumberExpr>(2.0));
        ASSERT_TRUE(SemanticAnalyzerConstEvalAccessorForTests::constEval(le.get(), out));
        EXPECT_DOUBLE_EQ(out, 1.0);
        auto gt  = std::make_unique<BinaryExpr>(BinaryOp::Gt, std::make_unique<NumberExpr>(3.0), std::make_unique<NumberExpr>(2.0));
        ASSERT_TRUE(SemanticAnalyzerConstEvalAccessorForTests::constEval(gt.get(), out));
        EXPECT_DOUBLE_EQ(out, 1.0);
        auto ge  = std::make_unique<BinaryExpr>(BinaryOp::Ge, std::make_unique<NumberExpr>(3.0), std::make_unique<NumberExpr>(3.0));
        ASSERT_TRUE(SemanticAnalyzerConstEvalAccessorForTests::constEval(ge.get(), out));
        EXPECT_DOUBLE_EQ(out, 1.0);
    }
    // Unsupported op should return false
    {
        auto pow = std::make_unique<BinaryExpr>(BinaryOp::Pow, std::make_unique<NumberExpr>(2.0), std::make_unique<NumberExpr>(3.0));
        EXPECT_FALSE(SemanticAnalyzerConstEvalAccessorForTests::constEval(pow.get(), out));
    }
}
