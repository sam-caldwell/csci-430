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
 * Test Suite: Parser Assign (LET)
 * Purpose: Ensure LET assignments parse with correct precedence (1 + 2*3).
 * Components Under Test: Lexer; Parser parseProgram/expressions/precedence.
 * Expected Behavior: Program has one AssignStmt with name A and value
 *          represented as BinaryExpr(Add, 1, BinaryExpr(Mul, 2, 3)).
 */
TEST(Parser, AssignWithLetAndPrecedence) {
    std::string src = "10 LET A = 1+2*3\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto [lines] = p.parseProgram();
    ASSERT_EQ(lines.size(), 1u);
    ASSERT_EQ(lines[0].statements.size(), 1u);
    auto* as = dynamic_cast<AssignStmt*>(lines[0].statements[0].get());
    ASSERT_NE(as, nullptr);
    EXPECT_EQ(as->name, "A");
    // Expect 1 + (2 * 3)
    auto* add = dynamic_cast<BinaryExpr*>(as->value.get());
    ASSERT_NE(add, nullptr);
    EXPECT_EQ(add->op, BinaryOp::Add);
    auto* one = dynamic_cast<NumberExpr*>(add->lhs.get());
    ASSERT_NE(one, nullptr);
    EXPECT_DOUBLE_EQ(one->value, 1.0);
    auto* mul = dynamic_cast<BinaryExpr*>(add->rhs.get());
    ASSERT_NE(mul, nullptr);
    EXPECT_EQ(mul->op, BinaryOp::Mul);
}
