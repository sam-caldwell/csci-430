// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"

using namespace gwbasic;
/*
 * Test Suite: Parser Error (missing ')'
 * Purpose: Ensure unmatched opening parenthesis triggers an error.
 * Components Under Test: Parser parseExpression/parseFactor.
 * Expected Behavior: Parsing fails when right parenthesis is missing.
 */
/***
Test: Parser.ErrorMissingRightParen
Inputs: BASIC source snippet
Code under test: Parser (BASIC → AST)
Expected behavior: AST structure or parse errors match expectations
*/
TEST(Parser, ErrorMissingRightParen) {
    std::string src = "10 PRINT (1+2\n"; // missing ')'
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    EXPECT_THROW({ auto prog = p.parseProgram(); (void)prog; }, ParseError);
}
