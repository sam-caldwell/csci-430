// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/parser/ParseError.h"

using namespace gwbasic;

/***
Test: Parser.Write_SadPaths
Inputs: WRITE #1 (missing list), WRITE , 1 (leading comma)
Expected: ParseError
*/
TEST(Parser, Write_SadPaths) {
    {
        const std::string src = "10 WRITE #1\n";
        std::istringstream iss(src);
        Lexer lex(iss);
        auto toks = lex.tokenize();
        Parser p(std::move(toks));
        EXPECT_THROW({ (void)p.parseProgram(); }, ParseError);
    }
    {
        const std::string src = "10 WRITE ,1\n";
        std::istringstream iss(src);
        Lexer lex(iss);
        auto toks = lex.tokenize();
        Parser p(std::move(toks));
        EXPECT_THROW({ (void)p.parseProgram(); }, ParseError);
    }
}

