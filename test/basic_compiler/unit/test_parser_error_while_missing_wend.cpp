// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"

using namespace gwbasic;

/***
 * Test: Parser.ErrorWhileMissingWendSameLine
 * Purpose: Validate that a WHILE without a corresponding to WEND on the same line is rejected.
 * Components Under Test: Lexer::tokenize; Parser::parseProgram
 * Expected Behavior: Parsing throws ParseError when a WHILE loop lacks a WEND terminator.
 */
TEST(Parser, ErrorWhileMissingWendSameLine) {
    std::string src = "10 WHILE I < 3 : PRINT I\n"; // missing WEND on the same line
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    EXPECT_THROW({ auto prog = p.parseProgram(); (void)prog; }, ParseError);
}
