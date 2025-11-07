// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/parser/ParseError.h"

using namespace gwbasic;

/***
 * Test: Parser.OnErrorGotoMissingLineErrors
 * Purpose: Validate ON ERROR GOTO without a line number triggers a parse error.
 * Components Under Test: Parser::parse ON ERROR GOTO.
 * Expected Behavior: Throws ParseError.
 */
TEST(Parser, OnErrorGotoMissingLineErrors) {
    std::string src = "10 ON ERROR GOTO\n";
    Lexer lx(src);
    auto toks = lx.tokenize();
    Parser p(std::move(toks));
    EXPECT_THROW({ (void)p.parseProgram(); }, ParseError);
}
