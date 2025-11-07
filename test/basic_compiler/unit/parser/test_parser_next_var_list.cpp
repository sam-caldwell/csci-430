// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/ForStmt.h"

/*
Test: Parser.NextVarList_ClosesNestedInOrder
Inputs: Program with nested FOR loops and a single NEXT with var-list
Expected behavior: parseProgram folds blocks without error; NEXT var-list
  closes inner then outer loops in order (equivalent to NEXT J : NEXT I).
*/
TEST(Parser, NextVarList_ClosesNestedInOrder) {
    const std::string src =
        "10 FOR I = 1 TO 2\n"
        "20 FOR J = 1 TO 2\n"
        "30 NEXT J, I\n";
    std::istringstream in(src);
    gwbasic::Lexer lx(in);
    auto toks = lx.tokenize();
    gwbasic::Parser p(std::move(toks));
    gwbasic::Program prog = p.parseProgram();
    ASSERT_EQ(prog.lines.size(), 1) << "FOR should fold into line 10";
    const auto &st = prog.lines[0].statements[0];
    auto *outer = dynamic_cast<gwbasic::ForStmt*>(st.get());
    ASSERT_NE(outer, nullptr);
    ASSERT_EQ(outer->var, "I");
    ASSERT_EQ(outer->body.size(), 1);
    auto *inner = dynamic_cast<gwbasic::ForStmt*>(outer->body[0].get());
    ASSERT_NE(inner, nullptr);
    ASSERT_EQ(inner->var, "J");
}

/*
Test: Parser.NextVarList_MismatchErrors
Inputs: Program with nested FOR loops and NEXT var-list in wrong order
Expected behavior: parseProgram throws due to mismatch with the innermost FOR.
*/
TEST(Parser, NextVarList_MismatchErrors) {
    const std::string src =
        "10 FOR I = 1 TO 1\n"
        "20 FOR J = 1 TO 1\n"
        "30 NEXT I, J\n"; // wrong order; first NEXT must match J
    std::istringstream in(src);
    gwbasic::Lexer lx(in);
    auto toks = lx.tokenize();
    gwbasic::Parser p(std::move(toks));
    EXPECT_THROW({ (void)p.parseProgram(); }, gwbasic::ParseError);
}

