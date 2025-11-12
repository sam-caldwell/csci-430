// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/parser/ParseError.h"

using namespace gwbasic;

/***
Test: Parser.ParseProgram_UnmatchedMarkers_ELSE
Purpose: Reject stray ELSE without an open IF block.
Components Under Test: Parser::parseProgram; marker validation.
Expected Behavior: Throws ParseError.
*/
TEST(Parser, ParseProgram_UnmatchedMarkers_ELSE) {
    std::string src = "10 ELSE\n";
    Lexer lx(src); auto toks = lx.tokenize();
    Parser p(std::move(toks));
    EXPECT_THROW({ auto _ = p.parseProgram(); }, ParseError);
}
