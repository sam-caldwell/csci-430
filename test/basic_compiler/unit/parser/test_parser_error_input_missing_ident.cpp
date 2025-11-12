// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"

using namespace gwbasic;
/*
 * Test Suite: Parser Error (INPUT missing ident)
 * Purpose: Ensure INPUT requires an identifier.
 * Components Under Test: Parser parseStatement for INPUT.
 * Expected Behavior: Error reported when no identifier follows INPUT.
 */
/***
Test: Parser.ErrorInputMissingIdentifier
Inputs: BASIC source snippet
Code under test: Parser (BASIC → AST)
Expected behavior: AST structure or parse errors match expectations
*/
TEST(Parser, ErrorInputMissingIdentifier) {
    std::string src = "10 INPUT 123\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    EXPECT_THROW({ auto prog = p.parseProgram(); (void)prog; }, ParseError);
}
