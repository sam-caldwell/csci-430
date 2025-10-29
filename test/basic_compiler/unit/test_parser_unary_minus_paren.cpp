// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/AssignStmt.h"
#include "basic_compiler/ast/BinaryExpr.h"
#include "basic_compiler/ast/UnaryExpr.h"
#include "basic_compiler/ast/NumberExpr.h"

using namespace gwbasic;
/*
 * Test Suite: Parser Unary (-(...))
 * Purpose: Validate unary minus on parenthesized expressions.
 * Components Under Test: Parser parseUnary/parseFactor.
 * Expected Behavior: UnaryExpr with op '-' over grouped inner expression.
 */
TEST(Parser, UnaryMinusParenthesized) {
    // A = -(1 + 2) * 3  => Mul( Unary(-, Add(1,2)), 3 )
    std::string src = "10 LET A = -(1 + 2) * 3\n";
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto [lines] = p.parseProgram();
    ASSERT_EQ(lines.size(), 1u);
    auto* as = dynamic_cast<AssignStmt*>(lines[0].statements[0].get());
    ASSERT_NE(as, nullptr);
    auto* mul = dynamic_cast<BinaryExpr*>(as->value.get());
    ASSERT_NE(mul, nullptr);
    EXPECT_EQ(mul->op, BinaryOp::Mul);
    auto* un = dynamic_cast<UnaryExpr*>(mul->lhs.get());
    ASSERT_NE(un, nullptr);
    EXPECT_EQ(un->op, '-');
    auto* three = dynamic_cast<NumberExpr*>(mul->rhs.get());
    ASSERT_NE(three, nullptr);
    EXPECT_DOUBLE_EQ(three->value, 3.0);
}
