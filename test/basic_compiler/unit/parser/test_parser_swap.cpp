// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/SwapStmt.h"

using namespace gwbasic;

/***
 * Test: Unit.Parser_SWAP_ParsesVarAndArray
 * Purpose: Ensure SWAP parses variable refs and array elements.
 * Components: Lexer, Parser
 * Expected: One SwapStmt with left=A and right=B$(I)
 */
/*
Test: Unit.Parser_SWAP_ParsesVarAndArray
Inputs: "10 SWAP A, B$(I)\n"
Code under test: Parser::parseSwap via parseStatement
Expected behavior: Program with line containing SwapStmt{left.name="A", right.name="B$" with 1 index}
*/
TEST(Unit, Parser_SWAP_ParsesVarAndArray) {
    std::string src = "10 SWAP A, B$(I)\n";
    Lexer lx(src);
    auto toks = lx.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();
    ASSERT_EQ(prog.lines.size(), 1u);
    const auto& line = prog.lines[0];
    ASSERT_EQ(line.statements.size(), 1u);
    auto* sw = dyn_cast<SwapStmt>(line.statements[0].get());
    ASSERT_NE(sw, nullptr);
    EXPECT_EQ(sw->left.name, "A");
    EXPECT_TRUE(sw->left.indices.empty());
    EXPECT_EQ(sw->right.name, "B$");
    ASSERT_EQ(sw->right.indices.size(), 1u);
}

