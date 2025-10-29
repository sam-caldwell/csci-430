// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/InputStmt.h"

using namespace gwbasic;
/*
 * Test Suite: Parser INPUT
 * Purpose: Validate parsing of INPUT statements consuming an identifier.
 * Components Under Test: Parser parseStatement for INPUT.
 * Expected Behavior: InputStmt present with expected variable name.
 */
TEST(Parser, InputStmt) {
    std::string src = "10 INPUT X\n";
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto [lines] = p.parseProgram();
    ASSERT_EQ(lines.size(), 1u);
    auto* is = dynamic_cast<InputStmt*>(lines[0].statements[0].get());
    ASSERT_NE(is, nullptr);
    EXPECT_EQ(is->name, "X");
}
