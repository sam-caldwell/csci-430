// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/parser/ParseError.h"

using namespace gwbasic;

TEST(Parser, Open_Errors_ModeAndChannel) {
    // Missing/invalid mode after FOR
    {
        std::string src = "10 OPEN \"f.txt\" FOR WHAT AS #1\n";
        Lexer lx(src); auto toks = lx.tokenize();
        Parser p(std::move(toks));
        EXPECT_THROW({ auto _ = p.parseProgram(); }, ParseError);
    }
    // Missing channel number after '#'
    {
        std::string src = "10 OPEN \"f.txt\" FOR INPUT AS #\n";
        Lexer lx(src); auto toks = lx.tokenize();
        Parser p(std::move(toks));
        EXPECT_THROW({ auto _ = p.parseProgram(); }, ParseError);
    }
}

