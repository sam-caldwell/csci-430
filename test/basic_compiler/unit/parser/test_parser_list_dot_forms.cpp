// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/ListStmt.h"

using namespace gwbasic;

/***
Test: Parser.LIST_Range_Variants
Inputs: LIST 100-200, LIST 100-, LIST 100
Code under test: Parser::parseList
Expected behavior: start/end values and dot flags parsed correctly
*/
TEST(Parser, LIST_Range_Variants) {
    const char* src =
        "10 LIST 100-200\n"
        "20 LIST 100-\n"
        "30 LIST 100\n";
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();
    ASSERT_EQ(prog.lines.size(), 3u);
    auto* l1 = dynamic_cast<ListStmt*>(prog.lines[0].statements[0].get());
    ASSERT_NE(l1, nullptr);
    EXPECT_FALSE(l1->startIsDot); EXPECT_FALSE(l1->endIsDot);
    EXPECT_EQ(l1->startLine.value_or(-1), 100); EXPECT_EQ(l1->endLine.value_or(-1), 200);

    auto* l2 = dynamic_cast<ListStmt*>(prog.lines[1].statements[0].get());
    ASSERT_NE(l2, nullptr);
    EXPECT_FALSE(l2->startIsDot); EXPECT_FALSE(l2->endIsDot);
    EXPECT_EQ(l2->startLine.value_or(-1), 100); EXPECT_FALSE(l2->endLine.has_value());

    auto* l3 = dynamic_cast<ListStmt*>(prog.lines[2].statements[0].get());
    ASSERT_NE(l3, nullptr);
    EXPECT_FALSE(l3->startIsDot); EXPECT_FALSE(l3->endIsDot);
    EXPECT_EQ(l3->startLine.value_or(-1), 100); EXPECT_EQ(l3->endLine.value_or(-1), 100);
}
