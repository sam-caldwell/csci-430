// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/AssignStmt.h"
#include "basic_compiler/ast/BinaryExpr.h"
#include "basic_compiler/ast/UnaryExpr.h"

using namespace gwbasic;
/*
 * Test Suite: Parser Unary (+x)
 * Purpose: Validate unary plus parsing as a no-op over expressions.
 * Components Under Test: Parser parseUnary.
 * Expected Behavior: UnaryExpr with op '+' returns the inner expression.
 */
TEST(Parser, UnaryPlusNoop) {
    // A = +1 * 2  => Mul( Unary('+',1), 2 )
    std::string src = "10 LET A = +1 * 2\n";
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
    EXPECT_EQ(un->op, '+');
}
