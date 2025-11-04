// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
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
    ASSERT_EQ(rd->targets[0].index, nullptr);
    ASSERT_EQ(rd->targets[1].index, nullptr);
    ASSERT_NE(rd->targets[2].index, nullptr);
    EXPECT_EQ(rd->targets[2].name, "C");
}

