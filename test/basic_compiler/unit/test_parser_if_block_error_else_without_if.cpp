// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"

using namespace gwbasic;

/***
 * Test: Parser.IfBlock_Error_ElseWithoutIf
 * Purpose: Validate that an ELSE without a preceding IF in a block context is rejected.
 * Components Under Test: Lexer::tokenize; Parser::parseProgram
 * Expected Behavior: Parsing throws ParseError when encountering ELSE without a matching IF.
 */
/*
Test: Parser.IfBlock_Error_ElseWithoutIf
Inputs: BASIC source snippet
Code under test: Parser (BASIC → AST)
Expected behavior: AST structure or parse errors match expectations
*/
TEST(Parser, IfBlock_Error_ElseWithoutIf) {
    std::string src = "10 ELSE\n";
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    EXPECT_THROW({ auto prog = p.parseProgram(); (void)prog; }, ParseError);
}
