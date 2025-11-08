// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/parser/ParseError.h"

using namespace gwbasic;

/***
Test: Parser.OptionPrintZones_SadPaths
Inputs: OPTION <wrong>; OPTION PRINTZONES <wrong token>
Expected: ParseError
*/
TEST(Parser, OptionPrintZones_SadPaths) {
    {
        const std::string src = "10 OPTION FOO\n";
        std::istringstream iss(src);
        Lexer lex(iss);
        auto toks = lex.tokenize();
        Parser p(std::move(toks));
        EXPECT_THROW({ (void)p.parseProgram(); }, ParseError);
    }
    {
        const std::string src = "10 OPTION PRINTZONES MAYBE\n";
        std::istringstream iss(src);
        Lexer lex(iss);
        auto toks = lex.tokenize();
        Parser p(std::move(toks));
        EXPECT_THROW({ (void)p.parseProgram(); }, ParseError);
    }
}

