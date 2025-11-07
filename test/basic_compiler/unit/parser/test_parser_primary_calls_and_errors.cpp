// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/PrintStmt.h"
#include "basic_compiler/ast/CallExpr.h"
#include "basic_compiler/ast/VarExpr.h"

using namespace gwbasic;

/*
Test: ParserPrimary.FunctionCall_Args_And_ParenExpr
Purpose: Cover identifier+call with args and parenthesized expression.
*/
TEST(ParserPrimary, FunctionCall_Args_And_ParenExpr) {
    const std::string src = "10 PRINT F(1, (2))\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    Program prog = p.parseProgram();
    ASSERT_EQ(prog.lines.size(), 1u);
    ASSERT_EQ(prog.lines[0].statements.size(), 1u);
    auto* ps = dyn_cast<PrintStmt>(prog.lines[0].statements[0].get());
    ASSERT_NE(ps, nullptr);
    auto* ce = dyn_cast<CallExpr>(ps->value.get());
    ASSERT_NE(ce, nullptr);
    EXPECT_EQ(ce->callee, "F");
    EXPECT_EQ(ce->args.size(), 2u);
}

/*
Test: ParserPrimary.ErrorOnUnexpectedToken
Purpose: Trigger the default error path in parsePrimary with an unexpected token.
*/
TEST(ParserPrimary, ErrorOnUnexpectedToken) {
    const std::string src = "10 PRINT )\n"; // unexpected
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    EXPECT_THROW({ (void)p.parseProgram(); }, ParseError);
}
