// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/parser/ParseError.h"

using namespace gwbasic;

/***
Test: Parser.CloseWithoutHash_Errors
Inputs: CLOSE (without '#n')
Code under test: Parser::parseStatement/parseClose
Expected behavior: ParseError thrown because CLOSE requires '#n'.
*/
TEST(Parser, CloseWithoutHash_Errors) {
    const std::string src = "10 CLOSE\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    EXPECT_THROW({ (void)p.parseProgram(); }, ParseError);
}

