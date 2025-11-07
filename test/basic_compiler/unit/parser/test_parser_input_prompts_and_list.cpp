// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/InputStmt.h"

using namespace gwbasic;

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

