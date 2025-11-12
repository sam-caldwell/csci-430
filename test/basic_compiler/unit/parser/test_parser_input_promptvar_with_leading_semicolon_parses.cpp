// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/InputStmt.h"

using namespace gwbasic;

/***
 * Test: Parser.Input_PromptVar_WithLeadingSemicolon_Parses
 * Purpose: Ensure INPUT ; P$, X parses with promptVar captured and variable list.
 * Components Under Test: Parser::parseInput (prompt variable).
 * Expected Behavior: InputStmt has promptVar=P$ and variables {X}.
 */
TEST(Parser, Input_PromptVar_WithLeadingSemicolon_Parses) {
    const std::string src = "10 INPUT ; P$, X\n";
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();
    ASSERT_EQ(prog.lines.size(), 1u);
    const auto* is = dynamic_cast<InputStmt*>(prog.lines[0].statements[0].get());
    ASSERT_NE(is, nullptr);
    ASSERT_TRUE(is->promptVar.has_value());
    EXPECT_EQ(*is->promptVar, "P$");
    ASSERT_EQ(is->variables.size(), 1u);
    EXPECT_EQ(is->variables[0], "X");
}
