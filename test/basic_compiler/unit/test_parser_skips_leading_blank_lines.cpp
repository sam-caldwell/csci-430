// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"

using namespace gwbasic;

/***
 * Test: Parser.SkipsLeadingBlankLines
 * Purpose: Validate that leading blank lines are ignored before the first program line.
 * Components Under Test: Lexer::tokenize; Parser::parseProgram
 * Expected Behavior: Parses a single line with number 10 despite leading newlines.
 */
/*
Test: Parser.SkipsLeadingBlankLines
Inputs: BASIC source snippet
Code under test: Parser (BASIC → AST)
Expected behavior: AST structure or parse errors match expectations
*/
TEST(Parser, SkipsLeadingBlankLines) {
    const std::string src = "\n\n10 END\n\n";
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();
    ASSERT_EQ(prog.lines.size(), 1u);
    EXPECT_EQ(prog.lines.front().number, 10);
}
