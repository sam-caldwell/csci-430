// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"

using namespace gwbasic;
/*
 * Test Suite: Parser Error (GOSUB missing line)
 * Purpose: Ensure GOSUB requires a numeric line target.
 * Components Under Test: Parser parseStatement for GOSUB.
 * Expected Behavior: Error reported when the line number is absent.
 */
/*
Test: Parser.ErrorGosubMissingLine
Inputs: BASIC source snippet
Code under test: Parser (BASIC → AST)
Expected behavior: AST structure or parse errors match expectations
*/
TEST(Parser, ErrorGosubMissingLine) {
    std::string src = "10 GOSUB\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    EXPECT_THROW({ auto prog = p.parseProgram(); (void)prog; }, ParseError);
}
