// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/PrintStmt.h"

using namespace gwbasic;

/***
Test: Parser.LPRINT_UsingInline_And_TrailingComma
Inputs: LPRINT USING "%d" 1;  and  LPRINT USING "%s", "A",
Code under test: Parser::parseLprint (USING within list; trailing comma behavior)
Expected behavior: format captured, items parsed, seps/trail set as expected
*/
TEST(Parser, LPRINT_UsingInline_And_TrailingComma) {
    const char* src =
        "10 LPRINT USING \"%d\" 1;\n"
        "20 LPRINT USING \"%s\", \"A\",\n";
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();
    ASSERT_EQ(prog.lines.size(), 2u);

    auto* p1 = dynamic_cast<PrintStmt*>(prog.lines[0].statements[0].get());
    ASSERT_NE(p1, nullptr);
    EXPECT_EQ(p1->channel, 1);
    ASSERT_NE(p1->format, nullptr);
    ASSERT_NE(p1->value, nullptr);
    EXPECT_EQ(p1->trail, PrintStmt::Terminator::Semicolon);

    auto* p2 = dynamic_cast<PrintStmt*>(prog.lines[1].statements[0].get());
    ASSERT_NE(p2, nullptr);
    EXPECT_EQ(p2->channel, 1);
    ASSERT_NE(p2->format, nullptr);
    ASSERT_NE(p2->value, nullptr);
    ASSERT_EQ(p2->more.size(), 0u);
    ASSERT_EQ(p2->seps.size(), 0u);
    EXPECT_EQ(p2->trail, PrintStmt::Terminator::Comma);
}
