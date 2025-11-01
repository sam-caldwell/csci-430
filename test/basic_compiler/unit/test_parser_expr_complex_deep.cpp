// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/AssignStmt.h"
#include "basic_compiler/ast/BinaryExpr.h"

using namespace gwbasic;
/*
 * Test Suite: Parser Expressions (complex)
 * Purpose: Validate parsing of deeply nested and mixed-precedence expressions.
 * Components Under Test: Parser parseExpression/term/factor/unary.
 * Expected Behavior: AST reflects correct associativity and precedence.
 */
TEST(Parser, ExprComplexDeep) {
    // A = (1 + 2) * (3 - (4 / (5 + 6)))
    std::string src = "10 LET A = (1 + 2) * (3 - (4 / (5 + 6)))\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto [lines] = p.parseProgram();
    ASSERT_EQ(lines.size(), 1u);
    auto* as = dynamic_cast<AssignStmt*>(lines[0].statements[0].get());
    ASSERT_NE(as, nullptr);
    auto* mul = dynamic_cast<BinaryExpr*>(as->value.get());
    ASSERT_NE(mul, nullptr);
    EXPECT_EQ(mul->op, BinaryOp::Mul);
    auto* left = dynamic_cast<BinaryExpr*>(mul->lhs.get());
    ASSERT_NE(left, nullptr);
    EXPECT_EQ(left->op, BinaryOp::Add);
    auto* right = dynamic_cast<BinaryExpr*>(mul->rhs.get());
    ASSERT_NE(right, nullptr);
    EXPECT_EQ(right->op, BinaryOp::Sub);
    auto* div = dynamic_cast<BinaryExpr*>(right->rhs.get());
    ASSERT_NE(div, nullptr);
    EXPECT_EQ(div->op, BinaryOp::Div);
    auto* addInner = dynamic_cast<BinaryExpr*>(div->rhs.get());
    ASSERT_NE(addInner, nullptr);
    EXPECT_EQ(addInner->op, BinaryOp::Add);
}
