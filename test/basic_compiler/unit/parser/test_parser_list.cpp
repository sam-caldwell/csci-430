// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/ListStmt.h"

using namespace gwbasic;

/***
 * Test: Parser.LIST_and_LLIST_Syntax
 * Purpose: Ensure LIST/LLIST with and without ranges parse into ListStmt
 * with correct toPrinter and bounds.
 */
TEST(Parser, LIST_and_LLIST_Syntax) {
    const char* src =
        "10 LIST\n"
        "20 LIST 100-200\n"
        "30 LLIST 150-\n";
    Lexer lx(src);
    auto toks = lx.tokenize();
    Parser p(std::move(toks));
    Program prog = p.parseProgram();
    ASSERT_EQ(prog.lines.size(), 3u);
    {
        auto* ls = dyn_cast<ListStmt>(prog.lines[0].statements[0].get());
        ASSERT_NE(ls, nullptr);
        EXPECT_FALSE(ls->toPrinter);
        EXPECT_FALSE(ls->startLine.has_value());
        EXPECT_FALSE(ls->endLine.has_value());
    }
    {
        auto* ls = dyn_cast<ListStmt>(prog.lines[1].statements[0].get());
        ASSERT_NE(ls, nullptr);
        EXPECT_FALSE(ls->toPrinter);
        ASSERT_TRUE(ls->startLine.has_value());
        ASSERT_TRUE(ls->endLine.has_value());
        EXPECT_EQ(*ls->startLine, 100);
        EXPECT_EQ(*ls->endLine, 200);
    }
    {
        auto* ls = dyn_cast<ListStmt>(prog.lines[2].statements[0].get());
        ASSERT_NE(ls, nullptr);
        EXPECT_TRUE(ls->toPrinter);
        ASSERT_TRUE(ls->startLine.has_value());
        EXPECT_EQ(*ls->startLine, 150);
        EXPECT_FALSE(ls->endLine.has_value()); // open-ended
    }
}

