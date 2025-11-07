// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/PrintStmt.h"
#include "basic_compiler/ast/CallExpr.h"

using namespace gwbasic;

/***
Test: ParserPrimary.FunctionCall_Args_And_ParenExpr
Purpose: Parse PRINT of a function call with arguments including a parenthesized expression.
Components Under Test: Parser::parsePrimary; function call and paren expr handling.
Expected Behavior: Produces CallExpr with callee name and two arguments.
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
