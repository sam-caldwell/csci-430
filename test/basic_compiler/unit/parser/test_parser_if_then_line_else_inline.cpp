// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/IfBlockStmt.h"
#include "basic_compiler/ast/GotoStmt.h"
#include "basic_compiler/ast/AssignStmt.h"
#include "basic_compiler/ast/PrintStmt.h"

using namespace gwbasic;

/***
Test: Parser.If_ThenLine_ElseInlineStmtList
Inputs: "IF A=0 THEN 100 ELSE A=1: PRINT A"
Code under test: Parser::parseIf
Expected behavior: Inline IfBlockStmt with thenBody=[GOTO 100], elseBody has two statements (Assign, Print)
*/
TEST(Parser, If_ThenLine_ElseInlineStmtList) {
    const char* src =
        "10 IF A=0 THEN 100 ELSE A=1: PRINT A\n"
        "20 END\n";
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();
    ASSERT_EQ(prog.lines.size(), 2u);
    ASSERT_EQ(prog.lines[0].statements.size(), 1u);
    auto* ib = dynamic_cast<IfBlockStmt*>(prog.lines[0].statements[0].get());
    ASSERT_NE(ib, nullptr);
    EXPECT_TRUE(ib->inlineEnd);
    ASSERT_EQ(ib->thenBody.size(), 1u);
    ASSERT_EQ(ib->elseBody.size(), 2u);
    auto* gt = dynamic_cast<GotoStmt*>(ib->thenBody[0].get());
    ASSERT_NE(gt, nullptr);
    EXPECT_EQ(gt->targetLine, 100);
    EXPECT_NE(dynamic_cast<AssignStmt*>(ib->elseBody[0].get()), nullptr);
    EXPECT_NE(dynamic_cast<PrintStmt*>(ib->elseBody[1].get()), nullptr);
}

