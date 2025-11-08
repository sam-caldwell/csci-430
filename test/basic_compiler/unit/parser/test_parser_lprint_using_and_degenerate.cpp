// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/PrintStmt.h"

using namespace gwbasic;

/***
Test: Parser.LPRINT_Using_And_Degenerate
Inputs: LPRINT USING "fmt" ;  and  LPRINT USING "fmt",
Code under test: Parser::parseLprint
Expected behavior: format set, no items, correct trailing terminators, channel=1
*/
TEST(Parser, LPRINT_Using_And_Degenerate) {
    const char* src =
        "10 LPRINT USING \"%d\" ;\n"
        "20 LPRINT USING \"%s\",\n";
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();
    ASSERT_EQ(prog.lines.size(), 2u);

    auto* p1 = dynamic_cast<PrintStmt*>(prog.lines[0].statements[0].get());
    ASSERT_NE(p1, nullptr);
    EXPECT_EQ(p1->channel, 1);
    ASSERT_NE(p1->format, nullptr);
    EXPECT_EQ(p1->value, nullptr);
    EXPECT_TRUE(p1->more.empty());
    // The leading USING consumed the separator; with no items parsed, the trailing
    // terminator remains default (newline).
    EXPECT_EQ(p1->trail, PrintStmt::Terminator::Newline);

    auto* p2 = dynamic_cast<PrintStmt*>(prog.lines[1].statements[0].get());
    ASSERT_NE(p2, nullptr);
    EXPECT_EQ(p2->channel, 1);
    ASSERT_NE(p2->format, nullptr);
    EXPECT_EQ(p2->value, nullptr);
    EXPECT_TRUE(p2->more.empty());
    EXPECT_EQ(p2->trail, PrintStmt::Terminator::Newline);
}
