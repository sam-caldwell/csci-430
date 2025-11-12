// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/InputStmt.h"
#include "basic_compiler/ast/LineInputStmt.h"

using namespace gwbasic;

/***
 * Test: Parser.LineInputParsesWithAndWithoutChannel
 * Purpose: Ensure LINE INPUT [#n,] var$ parses; channel optional.
 */
/*
Test: Parser.LineInputParsesWithAndWithoutChannel
Inputs: BASIC source snippet
Code under test: Parser (BASIC → AST)
Expected behavior: AST structure or parse errors match expectations
*/
TEST(Parser, LineInputParsesWithAndWithoutChannel) {
    {
        std::string src = "10 LINE INPUT A$\n";
        std::istringstream iss(src);
        Lexer lex(iss);
        auto toks = lex.tokenize();
        Parser p(std::move(toks));
        auto [lines] = p.parseProgram();
        ASSERT_EQ(lines.size(), 1u);
        auto* li = dynamic_cast<LineInputStmt*>(lines[0].statements[0].get());
        ASSERT_NE(li, nullptr);
        EXPECT_EQ(li->channel, -1);
        EXPECT_EQ(li->name, "A$");
    }
    {
        std::string src = "10 LINE INPUT #4, STR$\n";
        std::istringstream iss(src);
        Lexer lex(iss);
        auto toks = lex.tokenize();
        Parser p(std::move(toks));
        auto [lines] = p.parseProgram();
        ASSERT_EQ(lines.size(), 1u);
        auto* li = dynamic_cast<LineInputStmt*>(lines[0].statements[0].get());
        ASSERT_NE(li, nullptr);
        EXPECT_EQ(li->channel, 4);
        EXPECT_EQ(li->name, "STR$");
    }
}
