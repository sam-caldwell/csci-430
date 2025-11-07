// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/OnGotoStmt.h"
#include "basic_compiler/ast/OnGosubStmt.h"

using namespace gwbasic;

/***
Test: Parser.OnGotoGosub
Inputs: BASIC source snippets
Code under test: Parser (ON GOTO/GOSUB forms)
Expected behavior: AST nodes contain parsed index and target lists
*/
TEST(Parser, OnGotoGosub) {
    {
        std::string src = "10 ON A GOTO 100,200,300\n";
        Lexer lx(src); auto toks = lx.tokenize();
        Parser p(std::move(toks));
        auto [lines] = p.parseProgram();
        ASSERT_EQ(lines.size(), 1u);
        auto* s = dynamic_cast<OnGotoStmt*>(lines[0].statements[0].get());
        ASSERT_NE(s, nullptr);
        ASSERT_EQ(s->targets.size(), 3u);
        EXPECT_EQ(s->targets[0], 100);
        EXPECT_EQ(s->targets[1], 200);
        EXPECT_EQ(s->targets[2], 300);
    }
    {
        std::string src = "10 ON 2 GOSUB 10,20\n";
        Lexer lx(src); auto toks = lx.tokenize();
        Parser p(std::move(toks));
        auto [lines] = p.parseProgram();
        ASSERT_EQ(lines.size(), 1u);
        auto* s = dynamic_cast<OnGosubStmt*>(lines[0].statements[0].get());
        ASSERT_NE(s, nullptr);
        ASSERT_EQ(s->targets.size(), 2u);
        EXPECT_EQ(s->targets[0], 10);
        EXPECT_EQ(s->targets[1], 20);
    }
}
