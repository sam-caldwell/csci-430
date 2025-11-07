// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/FileInputStmt.h"

using namespace gwbasic;

/***
 * Test: Parser.Input_WithChannelAndList
 * Purpose: Ensure INPUT #n, varlist parses into FileInputStmt with channel and variables.
 * Components Under Test: Parser::parseInput (file channel form).
 * Expected Behavior: FileInputStmt has channel=3 and variables {A, B$}.
 */
TEST(Parser, Input_WithChannelAndList) {
    const std::string src = "10 INPUT #3, A, B$\n";
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();
    ASSERT_EQ(prog.lines.size(), 1u);
    const auto& line = prog.lines[0];
    ASSERT_EQ(line.statements.size(), 1u);
    const auto* fi = dynamic_cast<FileInputStmt*>(line.statements[0].get());
    ASSERT_NE(fi, nullptr);
    EXPECT_EQ(fi->channel, 3);
    ASSERT_EQ(fi->variables.size(), 2u);
    EXPECT_EQ(fi->variables[0], "A");
    EXPECT_EQ(fi->variables[1], "B$");
}
