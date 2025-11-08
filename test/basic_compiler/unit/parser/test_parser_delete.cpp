// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/DeleteStmt.h"

using namespace gwbasic;

/***
 * Test: Parser.DELETE_Syntax
 * Purpose: Ensure DELETE with single, ranged, open-ended, and '.' bounds
 * parses into DeleteStmt with correct flags and values.
 */
TEST(Parser, DELETE_Syntax) {
    const char* src =
        "10 DELETE 100\n"
        "20 DELETE 200-300\n"
        "30 DELETE 150-\n"
        "40 END\n";
    Lexer lx(src);
    auto toks = lx.tokenize();
    Parser p(std::move(toks));
    Program prog = p.parseProgram();
    ASSERT_EQ(prog.lines.size(), 4u);
    {
        auto* ds = dyn_cast<DeleteStmt>(prog.lines[0].statements[0].get());
        ASSERT_NE(ds, nullptr);
        ASSERT_TRUE(ds->startLine.has_value());
        ASSERT_TRUE(ds->endLine.has_value());
        EXPECT_EQ(*ds->startLine, 100);
        EXPECT_EQ(*ds->endLine, 100);
        EXPECT_FALSE(ds->startIsDot);
        EXPECT_FALSE(ds->endIsDot);
    }
    {
        auto* ds = dyn_cast<DeleteStmt>(prog.lines[1].statements[0].get());
        ASSERT_NE(ds, nullptr);
        ASSERT_TRUE(ds->startLine.has_value());
        ASSERT_TRUE(ds->endLine.has_value());
        EXPECT_EQ(*ds->startLine, 200);
        EXPECT_EQ(*ds->endLine, 300);
    }
    {
        auto* ds = dyn_cast<DeleteStmt>(prog.lines[2].statements[0].get());
        ASSERT_NE(ds, nullptr);
        ASSERT_TRUE(ds->startLine.has_value());
        EXPECT_EQ(*ds->startLine, 150);
        EXPECT_FALSE(ds->endLine.has_value()); // open-ended
    }
    // No "." variant supported by lexer; not tested here.
}
