// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/InputStmt.h"

using namespace gwbasic;

/***
 * Test: Parser.Input_VarList_Parses
 * Purpose: Ensure INPUT with a variable list parses and captures all identifiers.
 * Components Under Test: Parser::parseInput (console form).
 * Expected Behavior: InputStmt has three variables A, B, C in order.
 */
TEST(Parser, Input_VarList_Parses) {
    const std::string src = "10 INPUT A, B, C\n";
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();
    ASSERT_EQ(prog.lines.size(), 1u);
    const auto* is = dynamic_cast<InputStmt*>(prog.lines[0].statements[0].get());
    ASSERT_NE(is, nullptr);
    ASSERT_EQ(is->variables.size(), 3u);
    EXPECT_EQ(is->variables[0], "A");
    EXPECT_EQ(is->variables[1], "B");
    EXPECT_EQ(is->variables[2], "C");
}
