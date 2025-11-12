// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/lexer/Lexer.h"
#include "basic_compiler/ast/ColorStmt.h"

using namespace gwbasic;

/***
 * Test: Parser.ColorStatementParsesVariants
 * Inputs: Three COLOR statements with 1, 2, and 3 arguments.
 * Code under test: Lexer/Parser building ColorStmt nodes.
 * Expected behavior: Each line parses to ColorStmt with expected optional args.
 */
TEST(Parser, ColorStatementParsesVariants) {
    std::string src =
        "10 COLOR 2\n"
        "20 COLOR 2,4\n"
        "30 COLOR 3,5,0\n";
    Lexer lx(src);
    auto toks = lx.tokenize();
    Parser p(std::move(toks));
    Program prog = p.parseProgram();
    ASSERT_EQ(prog.lines.size(), 3u);
    auto *c1 = dynamic_cast<ColorStmt*>(prog.lines[0].statements[0].get());
    ASSERT_NE(c1, nullptr);
    EXPECT_NE(c1->fg, nullptr);
    EXPECT_EQ(c1->bg, nullptr);
    auto *c2 = dynamic_cast<ColorStmt*>(prog.lines[1].statements[0].get());
    ASSERT_NE(c2, nullptr);
    EXPECT_NE(c2->fg, nullptr);
    EXPECT_NE(c2->bg, nullptr);
    auto *c3 = dynamic_cast<ColorStmt*>(prog.lines[2].statements[0].get());
    ASSERT_NE(c3, nullptr);
    EXPECT_NE(c3->fg, nullptr);
    EXPECT_NE(c3->bg, nullptr);
    EXPECT_NE(c3->border, nullptr);
}
