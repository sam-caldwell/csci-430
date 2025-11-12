// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/parser/ParseError.h"

using namespace gwbasic;

/***
Test: Parser.OnGotoGosubErrors
Inputs: Malformed ON GOTO/GOSUB statements
Code under test: Parser::parseOnGotoGosub error branches
Expected behavior: Throws ParseError with descriptive messages
*/
TEST(Parser, OnGotoGosub_Errors) {
    // Missing GOTO/GOSUB keyword
    {
        std::string src = "10 ON A FOO 10\n";
        Lexer lx(src); auto toks = lx.tokenize();
        Parser p(std::move(toks));
        EXPECT_THROW({ auto _ = p.parseProgram(); }, ParseError);
    }
    // Missing first line number after GOTO
    {
        std::string src = "10 ON A GOTO\n";
        Lexer lx(src); auto toks = lx.tokenize();
        Parser p(std::move(toks));
        EXPECT_THROW({ auto _ = p.parseProgram(); }, ParseError);
    }
    // Missing subsequent line number in list
    {
        std::string src = "10 ON A GOTO 10, FOO\n";
        Lexer lx(src); auto toks = lx.tokenize();
        Parser p(std::move(toks));
        EXPECT_THROW({ auto _ = p.parseProgram(); }, ParseError);
    }
}
