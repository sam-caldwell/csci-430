// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/parser/ParseError.h"

using namespace gwbasic;

/***
Test: Parser.ParseProgram_UnmatchedMarkers_NEXT
Purpose: Reject stray NEXT without an open FOR.
Components Under Test: Parser::parseProgram; marker validation.
Expected Behavior: Throws ParseError.
*/
TEST(Parser, ParseProgram_UnmatchedMarkers_NEXT) {
    std::string src = "10 NEXT\n";
    Lexer lx(src); auto toks = lx.tokenize();
    Parser p(std::move(toks));
    EXPECT_THROW({ auto _ = p.parseProgram(); }, ParseError);
}
