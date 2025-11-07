// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/OpenStmt.h"

using namespace gwbasic;

/***
Test: Parser.Open_InputAndOutput
Purpose: Validate OPEN parses INPUT and OUTPUT modes with channel numbers.
Components Under Test: Parser::parseOpen; OpenStmt fields.
Expected Behavior: OpenStmt records mode and channel as parsed.
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
