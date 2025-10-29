// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/AssignStmt.h"
#include "basic_compiler/ast/BinaryExpr.h"
#include "basic_compiler/ast/NumberExpr.h"

using namespace gwbasic;
/*
 * Test Suite: Parser Expressions (nested parens)
 * Purpose: Ensure nested parentheses group expressions correctly.
 * Components Under Test: Parser parseFactor/parenthesized expressions.
 * Expected Behavior: AST nests BinaryExpr/UnaryExpr as dictated by parens.
 */
TEST(Parser, ExprNestedParentheses) {
    // A = (1 + (2 * (3 + 4)))
    std::string src = "10 LET A = (1 + (2 * (3 + 4)))\n";
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto [lines] = p.parseProgram();
    ASSERT_EQ(lines.size(), 1u);
    auto* as = dynamic_cast<AssignStmt*>(lines[0].statements[0].get());
    ASSERT_NE(as, nullptr);
    auto* addOuter = dynamic_cast<BinaryExpr*>(as->value.get());
    ASSERT_NE(addOuter, nullptr);
    EXPECT_EQ(addOuter->op, BinaryOp::Add);
    auto* one = dynamic_cast<NumberExpr*>(addOuter->lhs.get());
    ASSERT_NE(one, nullptr);
    EXPECT_DOUBLE_EQ(one->value, 1.0);
    auto* mul = dynamic_cast<BinaryExpr*>(addOuter->rhs.get());
    ASSERT_NE(mul, nullptr);
    EXPECT_EQ(mul->op, BinaryOp::Mul);
    auto* two = dynamic_cast<NumberExpr*>(mul->lhs.get());
    ASSERT_NE(two, nullptr);
    EXPECT_DOUBLE_EQ(two->value, 2.0);
    auto* addInner = dynamic_cast<BinaryExpr*>(mul->rhs.get());
    ASSERT_NE(addInner, nullptr);
    EXPECT_EQ(addInner->op, BinaryOp::Add);
}
