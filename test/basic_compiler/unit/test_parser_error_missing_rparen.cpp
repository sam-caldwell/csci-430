// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"

using namespace gwbasic;
/*
 * Test Suite: Parser Error (missing ')')
 * Purpose: Ensure unmatched opening parenthesis triggers an error.
 * Components Under Test: Parser parseExpression/parseFactor.
 * Expected Behavior: Parsing fails when right parenthesis is missing.
 */
TEST(Parser, ErrorMissingRightParen) {
    std::string src = "10 PRINT (1+2\n"; // missing ')'
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    EXPECT_THROW({ auto prog = p.parseProgram(); (void)prog; }, ParseError);
}
