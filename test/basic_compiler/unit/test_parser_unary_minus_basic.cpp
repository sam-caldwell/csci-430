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
 * Test Suite: Parser Unary (-x)
 * Purpose: Validate unary minus parsing on simple numeric expressions.
 * Components Under Test: Parser parseUnary.
 * Expected Behavior: UnaryExpr with op '-' wrapping inner NumberExpr/VarExpr.
 */
TEST(Parser, UnaryMinusBasic) {
    // A = -1 + 2  => Add( Unary(-,1), 2 )
    std::string src = "10 LET A = -1 + 2\n";
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto [lines] = p.parseProgram();
    ASSERT_EQ(lines.size(), 1u);
    auto* as = dynamic_cast<AssignStmt*>(lines[0].statements[0].get());
    ASSERT_NE(as, nullptr);
    auto* add = dynamic_cast<BinaryExpr*>(as->value.get());
    ASSERT_NE(add, nullptr);
    EXPECT_EQ(add->op, BinaryOp::Add);
    auto* un = dynamic_cast<UnaryExpr*>(add->lhs.get());
    ASSERT_NE(un, nullptr);
    EXPECT_EQ(un->op, '-');
    auto* num2 = dynamic_cast<NumberExpr*>(add->rhs.get());
    ASSERT_NE(num2, nullptr);
    EXPECT_DOUBLE_EQ(num2->value, 2.0);
}
