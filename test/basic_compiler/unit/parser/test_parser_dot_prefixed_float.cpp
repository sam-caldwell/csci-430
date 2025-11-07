// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/AssignStmt.h"
#include "basic_compiler/ast/NumberExpr.h"

using namespace gwbasic;

/*
Test: Parser.DotPrefixedFloat_ParsesToNumberExpr
Inputs: Program with LET A = .5
Code under test: Lexer+Parser creating NumberExpr from .5 literal
Expected behavior: NumberExpr value equals 0.5
*/
TEST(Parser, DotPrefixedFloat_ParsesToNumberExpr) {
    const std::string src = "10 LET A = .5\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    Program prog = p.parseProgram();
    ASSERT_EQ(prog.lines.size(), 1u);
    ASSERT_EQ(prog.lines[0].statements.size(), 1u);
    auto* asg = dyn_cast<AssignStmt>(prog.lines[0].statements[0].get());
    ASSERT_NE(asg, nullptr);
    auto* num = dyn_cast<NumberExpr>(asg->value.get());
    ASSERT_NE(num, nullptr);
    EXPECT_DOUBLE_EQ(num->value, 0.5);
}

