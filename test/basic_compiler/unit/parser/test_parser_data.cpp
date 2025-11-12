// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/DataStmt.h"

using namespace gwbasic;

/***
 * Test: Parser.DataParsesLiteralList
 * Purpose: Ensure DATA parses comma-separated string and numeric literals.
 */
/*
Test: Parser.DataParsesLiteralList
Inputs: BASIC source snippet
Code under test: Parser (BASIC → AST)
Expected behavior: AST structure or parse errors match expectations
*/
TEST(Parser, DataParsesLiteralList) {
    std::string src = "10 DATA \"X\", 3.14, 42\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto [lines] = p.parseProgram();
    ASSERT_EQ(lines.size(), 1u);
    ASSERT_EQ(lines[0].statements.size(), 1u);
    auto* ds = dynamic_cast<DataStmt*>(lines[0].statements[0].get());
    ASSERT_NE(ds, nullptr);
    ASSERT_EQ(ds->items.size(), 3u);
    EXPECT_TRUE(ds->items[0].isString);
    EXPECT_EQ(ds->items[0].text, "X");
    EXPECT_FALSE(ds->items[1].isString);
    EXPECT_EQ(ds->items[1].text, "3.14");
    EXPECT_FALSE(ds->items[2].isString);
    EXPECT_EQ(ds->items[2].text, "42");
}
