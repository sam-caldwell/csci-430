// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include <sstream>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"

using namespace gwbasic;

/***
 * Test: Parser.ErrorMissingLeadingLineNumber
 * Purpose: Ensure parser rejects programs without an initial line number.
 */
/*
Test: Parser.ErrorMissingLeadingLineNumber
Inputs: BASIC source snippet
Code under test: Parser (BASIC → AST)
Expected behavior: AST structure or parse errors match expectations
*/
TEST(Parser, ErrorMissingLeadingLineNumber) {
    const std::string src = "PRINT 1\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    EXPECT_THROW({ auto prog = p.parseProgram(); (void)prog; }, ParseError);
}
