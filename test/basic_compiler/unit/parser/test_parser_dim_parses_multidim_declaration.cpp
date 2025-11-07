// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/DimStmt.h"

using namespace gwbasic;

/***
 * Test: Parser.DimParsesMultiDimDeclaration
 * Purpose: Ensure DIM A(3,5) parses and records both upper bounds.
 * Components Under Test: Parser::parse DIM statement.
 * Expected Behavior: DimStmt has upperBounds {3,5}.
 */
TEST(Parser, DimParsesMultiDimDeclaration) {
    std::string src = "10 DIM A(3,5)\n";
    Lexer lex(src); auto toks = lex.tokenize(); Parser p(std::move(toks));
    auto prog = p.parseProgram();
    ASSERT_EQ(prog.lines.size(), 1u);
    auto* ds = dynamic_cast<DimStmt*>(prog.lines[0].statements[0].get());
    ASSERT_NE(ds, nullptr);
    EXPECT_EQ(ds->name, "A");
    ASSERT_EQ(ds->upperBounds.size(), 2u);
    EXPECT_EQ(ds->upperBounds[0], 3);
    EXPECT_EQ(ds->upperBounds[1], 5);
}
