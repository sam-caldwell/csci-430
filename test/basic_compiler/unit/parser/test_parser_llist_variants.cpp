// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/ListStmt.h"

using namespace gwbasic;

/***
Test: Parser.LLIST_Variants
Inputs: LLIST, LLIST 200, LLIST 200-300, LLIST 200-
Code under test: Parser::parseLlist
Expected behavior: start/end parsed; toPrinter=true; open/end ranges handled
*/
TEST(Parser, LLIST_Variants) {
    const char* src =
        "10 LLIST\n"
        "20 LLIST 200\n"
        "30 LLIST 200-300\n"
        "40 LLIST 200-\n";
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();
    ASSERT_EQ(prog.lines.size(), 4u);

    auto* a = dynamic_cast<ListStmt*>(prog.lines[0].statements[0].get());
    ASSERT_NE(a, nullptr);
    EXPECT_TRUE(a->toPrinter);
    EXPECT_FALSE(a->startLine.has_value());
    EXPECT_FALSE(a->endLine.has_value());

    auto* b = dynamic_cast<ListStmt*>(prog.lines[1].statements[0].get());
    ASSERT_NE(b, nullptr);
    EXPECT_TRUE(b->toPrinter);
    EXPECT_EQ(b->startLine.value_or(-1), 200);
    EXPECT_EQ(b->endLine.value_or(-1), 200);

    auto* c = dynamic_cast<ListStmt*>(prog.lines[2].statements[0].get());
    ASSERT_NE(c, nullptr);
    EXPECT_TRUE(c->toPrinter);
    EXPECT_EQ(c->startLine.value_or(-1), 200);
    EXPECT_EQ(c->endLine.value_or(-1), 300);

    auto* d = dynamic_cast<ListStmt*>(prog.lines[3].statements[0].get());
    ASSERT_NE(d, nullptr);
    EXPECT_TRUE(d->toPrinter);
    EXPECT_EQ(d->startLine.value_or(-1), 200);
    EXPECT_FALSE(d->endLine.has_value());
}

