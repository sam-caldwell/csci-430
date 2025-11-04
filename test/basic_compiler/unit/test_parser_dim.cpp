// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/DimStmt.h"

using namespace gwbasic;

/***
 * Test: Parser.DimParsesArrayDeclaration
 * Purpose: Ensure DIM A(10) parses into DimStmt with correct name/length.
 */
/*
Test: Parser.DimParsesArrayDeclaration
Inputs: BASIC source snippet
Code under test: Parser (BASIC → AST)
Expected behavior: AST structure or parse errors match expectations
*/
TEST(Parser, DimParsesArrayDeclaration) {
    std::string src = "10 DIM A(10)\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto [lines] = p.parseProgram();
    ASSERT_EQ(lines.size(), 1u);
    ASSERT_EQ(lines[0].statements.size(), 1u);
    auto* ds = dynamic_cast<DimStmt*>(lines[0].statements[0].get());
    ASSERT_NE(ds, nullptr);
    EXPECT_EQ(ds->name, "A");
    EXPECT_EQ(ds->length, 10);
}

