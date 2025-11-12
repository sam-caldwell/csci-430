// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"

using namespace gwbasic;
/*
 * Test Suite: Parser Error (FOR missing ident)
 * Purpose: Ensure parser requires an induction variable name in FOR.
 * Components Under Test: Parser parseFor.
 * Expected Behavior: Parsing indicates an error when the identifier is absent.
 */
/***
Test: Parser.ErrorForMissingIdentifier
Inputs: BASIC source snippet
Code under test: Parser (BASIC → AST)
Expected behavior: AST structure or parse errors match expectations
*/
TEST(Parser, ErrorForMissingIdentifier) {
    std::string src = "10 FOR = 1 TO 2 : NEXT\n"; // no variable name after FOR
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    EXPECT_THROW({ auto prog = p.parseProgram(); (void)prog; }, ParseError);
}
