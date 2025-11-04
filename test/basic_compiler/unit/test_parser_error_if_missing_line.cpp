// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"

using namespace gwbasic;
/*
 * Test Suite: Parser Error (IF missing line)
 * Purpose: Ensure THEN must be followed by a line number.
 * Components Under Test: Parser parseIf/parseComparison.
 * Expected Behavior: Error reported when IF lacks a THEN target line.
 */
/*
Test: Parser.ErrorIfMissingLineNumber
Inputs: BASIC source snippet
Code under test: Parser (BASIC → AST)
Expected behavior: AST structure or parse errors match expectations
*/
TEST(Parser, ErrorIfMissingLineNumber) {
    std::string src = "10 IF A > 1 THEN PRINT 5\n"; // not a line number
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    EXPECT_THROW({ auto prog = p.parseProgram(); (void)prog; }, ParseError);
}
