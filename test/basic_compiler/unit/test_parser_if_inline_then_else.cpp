// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/ast/IfBlockStmt.h"
#include "basic_compiler/ast/GotoStmt.h"

using namespace gwbasic;

/***
 * Test: Parser.IfInlineThenElse.ParsesStatementLists
 * Purpose: Verify inline IF ... THEN stmtlist [ELSE stmtlist] parses to IfBlockStmt with bodies.
 */
/*
Test: Parser.IfInlineThenElse.ParsesStatementLists
Inputs: Single line with inline THEN/ELSE statement lists
Code under test: Parser::parseProgram (line/statement parsing and IF handler)
Expected behavior: Produces IfBlockStmt with thenBody and elseBody sizes matching input
*/
TEST(Parser, IfInlineThenElse_ParsesStatementLists) {
    const char* src =
        "10 IF A < 10 THEN A=1: PRINT A ELSE GOTO 200\n"
        "20 END\n";
    Lexer lex(src);
    auto tokens = lex.tokenize();
    Parser p(std::move(tokens));
    auto prog = p.parseProgram();
    ASSERT_EQ(prog.lines.size(), 2);
    ASSERT_EQ(prog.lines[0].statements.size(), 1);
    auto* ib = dynamic_cast<IfBlockStmt*>(prog.lines[0].statements[0].get());
    ASSERT_NE(ib, nullptr);
    EXPECT_TRUE(ib->inlineEnd);
    ASSERT_EQ(ib->thenBody.size(), 2); // A=1 and PRINT A
    ASSERT_EQ(ib->elseBody.size(), 1);
    auto* gt = dynamic_cast<GotoStmt*>(ib->elseBody[0].get());
    ASSERT_NE(gt, nullptr);
    EXPECT_EQ(gt->targetLine, 200);
}

