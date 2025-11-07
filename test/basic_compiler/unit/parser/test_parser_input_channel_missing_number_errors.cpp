// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/parser/ParseError.h"

using namespace gwbasic;

/***
 * Test: Parser.Input_ChannelMissingNumber_Errors
 * Purpose: Validate INPUT #, ... triggers a parse error due to missing channel number.
 * Components Under Test: Parser::parseInput error path.
 * Expected Behavior: Throws ParseError.
 */
TEST(Parser, Input_ChannelMissingNumber_Errors) {
    const std::string src = "10 INPUT #, A\n";
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    EXPECT_THROW({ (void)p.parseProgram(); }, ParseError);
}
