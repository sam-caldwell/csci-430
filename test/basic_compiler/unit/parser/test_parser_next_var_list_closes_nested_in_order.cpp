// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/ForStmt.h"

/***
 * Test: Parser.NextVarList_ClosesNestedInOrder
 * Purpose: Verify NEXT J, I closes nested FOR loops in the proper order.
 * Components Under Test: Parser::parseProgram folding of FOR/NEXT with var-list.
 * Expected Behavior: Outer FOR is for I; inner FOR is for J.
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
