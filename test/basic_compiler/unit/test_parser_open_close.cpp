// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/OpenStmt.h"
#include "basic_compiler/ast/CloseStmt.h"

using namespace gwbasic;

/***
 * Test: Parser.OpenCloseParsesFileOps
 * Purpose: Ensure OPEN ... FOR INPUT AS #n and CLOSE #n parse correctly.
 */
/*
Test: Parser.OpenCloseParsesFileOps
Inputs: BASIC source snippet
Code under test: Parser (BASIC → AST)
Expected behavior: AST structure or parse errors match expectations
*/
TEST(Parser, OpenCloseParsesFileOps) {
    std::string src = "10 OPEN \"out.txt\" FOR INPUT AS #3\n20 CLOSE #3\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto [lines] = p.parseProgram();
    ASSERT_EQ(lines.size(), 2u);
    ASSERT_EQ(lines[0].statements.size(), 1u);
    ASSERT_EQ(lines[1].statements.size(), 1u);
    auto* op = dynamic_cast<OpenStmt*>(lines[0].statements[0].get());
    ASSERT_NE(op, nullptr);
    EXPECT_EQ(op->mode, FileMode::Input);
    EXPECT_EQ(op->channel, 3);
    auto* cl = dynamic_cast<CloseStmt*>(lines[1].statements[0].get());
    ASSERT_NE(cl, nullptr);
    EXPECT_EQ(cl->channel, 3);
}
