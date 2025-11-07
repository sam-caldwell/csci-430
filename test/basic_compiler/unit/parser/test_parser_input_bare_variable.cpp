// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/InputStmt.h"

using namespace gwbasic;

/***
 * Test: Parser.Input_BareVariable
 * Purpose: Ensure bare console INPUT var$ parses as InputStmt with one variable.
 * Components Under Test: Parser::parseInput (console form).
 * Expected Behavior: InputStmt has variables {X$}.
 */
TEST(Parser, Input_BareVariable) {
    const std::string src = "10 INPUT X$\n";
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();
    ASSERT_EQ(prog.lines.size(), 1u);
    const auto& line = prog.lines[0];
    ASSERT_EQ(line.statements.size(), 1u);
    const auto* is = dynamic_cast<InputStmt*>(line.statements[0].get());
    ASSERT_NE(is, nullptr);
    ASSERT_EQ(is->variables.size(), 1u);
    EXPECT_EQ(is->variables[0], std::string("X$"));
}
