// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/Program.h"
#include "basic_compiler/ast/Line.h"
#include "basic_compiler/ast/InputStmt.h"
#include "basic_compiler/ast/FileInputStmt.h"

using namespace gwbasic;

/*
 * Test: Parser.Input_WithChannelAndList
 * Purpose: Parse INPUT #n, A, B$ into FileInputStmt with channel and var list.
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

/*
 * Test: Parser.Input_BareVariable
 * Purpose: Parse console INPUT X$ into InputStmt.
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

/*
 * Test: Parser.Input_ChannelMissingNumber_Errors
 * Purpose: Ensure INPUT #, ... triggers ParseError.
 */
TEST(Parser, Input_ChannelMissingNumber_Errors) {
    const std::string src = "10 INPUT #, A\n";
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    EXPECT_THROW({ (void)p.parseProgram(); }, ParseError);
}
