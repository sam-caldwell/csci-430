// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"

using namespace gwbasic;
/*
 * Test Suite: Parser Error (GOTO missing line)
 * Purpose: Ensure GOTO requires a numeric line target.
 * Components Under Test: Parser parseStatement for GOTO.
 * Expected Behavior: Error reported when the line number is absent.
 */
/***
Test: Parser.ErrorGotoMissingLine
Inputs: BASIC source snippet
Code under test: Parser (BASIC → AST)
Expected behavior: AST structure or parse errors match expectations
*/
TEST(Parser, ErrorGotoMissingLine) {
    std::string src = "10 GOTO\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    EXPECT_THROW({ auto prog = p.parseProgram(); (void)prog; }, ParseError);
}
