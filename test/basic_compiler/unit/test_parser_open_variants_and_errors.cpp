// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/OpenStmt.h"
#include "basic_compiler/parser/ParseError.h"

using namespace gwbasic;

/*
Test: Parser.OpenVariantsAndErrors
Inputs: OPEN with INPUT/OUTPUT modes and malformed forms
Code under test: Parser::parseOpen
Expected behavior: AST fields set correctly; malformed inputs throw
*/
TEST(Parser, Open_InputAndOutput) {
    {
        std::string src = "10 OPEN \"f.txt\" FOR INPUT AS #2\n";
        Lexer lx(src); auto toks = lx.tokenize();
        Parser p(std::move(toks));
        auto [lines] = p.parseProgram();
        ASSERT_EQ(lines.size(), 1u);
        auto* s = dynamic_cast<OpenStmt*>(lines[0].statements[0].get());
        ASSERT_NE(s, nullptr);
        EXPECT_EQ(s->mode, FileMode::Input);
        EXPECT_EQ(s->channel, 2);
    }
    {
        std::string src = "10 OPEN \"f.txt\" FOR OUTPUT AS #5\n";
        Lexer lx(src); auto toks = lx.tokenize();
        Parser p(std::move(toks));
        auto [lines] = p.parseProgram();
        ASSERT_EQ(lines.size(), 1u);
        auto* s = dynamic_cast<OpenStmt*>(lines[0].statements[0].get());
        ASSERT_NE(s, nullptr);
        EXPECT_EQ(s->mode, FileMode::Output);
        EXPECT_EQ(s->channel, 5);
    }
}

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

