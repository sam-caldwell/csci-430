// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/DimStmt.h"

using namespace gwbasic;

/***
 * Test: Parser.DimParsesArrayDeclaration
 * Purpose: Ensure DIM A(10) parses and records upper bound.
 * Components Under Test: Parser::parse DIM statement.
 * Expected Behavior: DimStmt has name A and upperBounds {10}.
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
    ASSERT_EQ(ds->upperBounds.size(), 1u);
    EXPECT_EQ(ds->upperBounds[0], 10);
}
