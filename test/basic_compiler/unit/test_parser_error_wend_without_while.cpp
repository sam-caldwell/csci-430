// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"

using namespace gwbasic;

/***
 * Test: Parser.ErrorWendWithoutWhile
 * Purpose: Validate that a WEND without a matching WHILE is rejected.
 * Components Under Test: Lexer::tokenize; Parser::parseProgram
 * Expected Behavior: Parsing throws ParseError when encountering WEND without WHILE.
 */
TEST(Parser, ErrorWendWithoutWhile) {
    std::string src = "10 WEND\n";
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    EXPECT_THROW({ auto prog = p.parseProgram(); (void)prog; }, ParseError);
}
