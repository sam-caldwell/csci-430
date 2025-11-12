// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/IfBlockStmt.h"

using namespace gwbasic;

/***
 * Test: Parser.IfInlineThenOnly.ParsesThenList
 * Purpose: Verify inline IF ... THEN stmtlist (no ELSE) parses correctly.
 */
/*
Test: Parser.IfInlineThenOnly.ParsesThenList
Inputs: Single line with inline THEN statement list (no ELSE)
Code under test: Parser::parseProgram
Expected behavior: IfBlockStmt with inlineEnd=true, thenBody size=2, elseBody size=0
*/
TEST(Parser, IfInlineThenOnly_ParsesThenList) {
    const char* src =
        "10 IF X = 1 THEN A=2: PRINT A\n"
        "20 END\n";
    Lexer lex(src);
    auto tokens = lex.tokenize();
    Parser p(std::move(tokens));
    auto prog = p.parseProgram();
    ASSERT_EQ(prog.lines.size(), 2u);
    ASSERT_EQ(prog.lines[0].statements.size(), 1u);
    auto* ib = dynamic_cast<IfBlockStmt*>(prog.lines[0].statements[0].get());
    ASSERT_NE(ib, nullptr);
    EXPECT_TRUE(ib->inlineEnd);
    EXPECT_EQ(ib->thenBody.size(), 2u);
    EXPECT_TRUE(ib->elseBody.empty());
}

