// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/ReadStmt.h"

using namespace gwbasic;

/***
 * Test: Parser.ReadParsesTargets
 * Purpose: Ensure READ parses variables and array element targets.
 */
/*
Test: Parser.ReadParsesTargets
Inputs: BASIC source snippet
Code under test: Parser (BASIC → AST)
Expected behavior: AST structure or parse errors match expectations
*/
TEST(Parser, ReadParsesTargets) {
    std::string src = "10 READ A, B, C(2)\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto [lines] = p.parseProgram();
    ASSERT_EQ(lines.size(), 1u);
    ASSERT_EQ(lines[0].statements.size(), 1u);
    auto* rd = dynamic_cast<ReadStmt*>(lines[0].statements[0].get());
    ASSERT_NE(rd, nullptr);
    ASSERT_EQ(rd->targets.size(), 3u);
    EXPECT_EQ(rd->targets[0].name, "A");
    EXPECT_EQ(rd->targets[1].name, "B");
    ASSERT_TRUE(rd->targets[0].indices.empty());
    ASSERT_TRUE(rd->targets[1].indices.empty());
    ASSERT_EQ(rd->targets[2].indices.size(), 1u);
    EXPECT_EQ(rd->targets[2].name, "C");
}
