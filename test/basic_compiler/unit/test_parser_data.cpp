// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
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
    EXPECT_EQ(ds->items[0], "X");
    EXPECT_EQ(ds->items[1], "3.14");
    EXPECT_EQ(ds->items[2], "42");
}

