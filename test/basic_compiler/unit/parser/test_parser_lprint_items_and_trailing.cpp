// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/PrintStmt.h"

using namespace gwbasic;

/***
Test: Parser.LPRINT_Items_And_Trailing
Inputs: LPRINT;   LPRINT,   LPRINT "A";   LPRINT "B", 1
Code under test: Parser::parseLprint
Expected behavior: Channel=1; trail set on degenerate cases; seps reflect comma/semicolon; items parsed
*/
TEST(Parser, LPRINT_Items_And_Trailing) {
    const char* src =
        "10 LPRINT;\n"
        "20 LPRINT,\n"
        "30 LPRINT \"A\";\n"
        "40 LPRINT \"B\", 1\n";
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();
    ASSERT_EQ(prog.lines.size(), 4u);

    auto* p1 = dynamic_cast<PrintStmt*>(prog.lines[0].statements[0].get());
    ASSERT_NE(p1, nullptr);
    EXPECT_EQ(p1->channel, 1);
    EXPECT_EQ(p1->value, nullptr);
    EXPECT_TRUE(p1->more.empty());
    EXPECT_EQ(p1->trail, PrintStmt::Terminator::Semicolon);

    auto* p2 = dynamic_cast<PrintStmt*>(prog.lines[1].statements[0].get());
    ASSERT_NE(p2, nullptr);
    EXPECT_EQ(p2->channel, 1);
    EXPECT_EQ(p2->value, nullptr);
    EXPECT_TRUE(p2->more.empty());
    EXPECT_EQ(p2->trail, PrintStmt::Terminator::Comma);

    auto* p3 = dynamic_cast<PrintStmt*>(prog.lines[2].statements[0].get());
    ASSERT_NE(p3, nullptr);
    EXPECT_EQ(p3->channel, 1);
    ASSERT_NE(p3->value, nullptr);
    ASSERT_EQ(p3->more.size(), 0u);
    ASSERT_EQ(p3->seps.size(), 0u);
    EXPECT_EQ(p3->trail, PrintStmt::Terminator::Semicolon);

    auto* p4 = dynamic_cast<PrintStmt*>(prog.lines[3].statements[0].get());
    ASSERT_NE(p4, nullptr);
    EXPECT_EQ(p4->channel, 1);
    ASSERT_NE(p4->value, nullptr);
    ASSERT_EQ(p4->more.size(), 1u);
    ASSERT_EQ(p4->seps.size(), 1u);
    EXPECT_EQ(p4->seps[0], PrintStmt::Sep::Comma);
    EXPECT_EQ(p4->trail, PrintStmt::Terminator::Newline);
}
