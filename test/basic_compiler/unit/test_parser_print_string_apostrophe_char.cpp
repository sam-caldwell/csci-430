// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include <sstream>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/PrintStmt.h"
#include "basic_compiler/ast/StringExpr.h"

using namespace gwbasic;

/***
 * Test: Parser.PrintString_WithApostropheChar
 * Purpose: Ensure a literal apostrophe (') inside a double-quoted string is
 *          not treated as a comment and remains part of the string.
 * Components: Lexer (string literal), Parser (PRINT)
 * Expected: StringExpr value equals "Bob's".
 */
/*
Test: Parser.PrintString_WithApostropheChar
Inputs: 10 PRINT "Bob's"\n
Code under test: Lexer::stringLiteral, Parser::parsePrint
Expected behavior: StringExpr contains Bob's (apostrophe preserved); no truncation
*/
TEST(Parser, PrintString_WithApostropheChar) {
    const std::string src = "10 PRINT \"Bob's\"\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();
    ASSERT_EQ(prog.lines.size(), 1u);
    ASSERT_EQ(prog.lines[0].statements.size(), 1u);
    auto* ps = dynamic_cast<PrintStmt*>(prog.lines[0].statements[0].get());
    ASSERT_NE(ps, nullptr);
    auto* se = dynamic_cast<StringExpr*>(ps->value.get());
    ASSERT_NE(se, nullptr);
    EXPECT_EQ(se->value, "Bob's");
}

