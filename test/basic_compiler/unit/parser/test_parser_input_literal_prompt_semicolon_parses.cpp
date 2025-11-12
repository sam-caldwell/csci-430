// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/InputStmt.h"

using namespace gwbasic;

/***
 * Test: Parser.Input_LiteralPrompt_Semicolon_Parses
 * Purpose: Verify INPUT "literal"; varlist form parses with prompt literal.
 * Components Under Test: Parser::parseInput (literal prompt).
 * Expected Behavior: InputStmt has promptLiteral and expected variables.
 */
TEST(Parser, Input_LiteralPrompt_Semicolon_Parses) {
    const std::string src = "10 INPUT \"N M:\"; N, M\n";
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();
    ASSERT_EQ(prog.lines.size(), 1u);
    const auto* is = dynamic_cast<InputStmt*>(prog.lines[0].statements[0].get());
    ASSERT_NE(is, nullptr);
    ASSERT_TRUE(is->promptLiteral.has_value());
    EXPECT_EQ(*is->promptLiteral, "N M:");
    ASSERT_EQ(is->variables.size(), 2u);
}
