// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/IfBlockStmt.h"
#include "basic_compiler/ast/GotoStmt.h"

using namespace gwbasic;

/***
Test: Parser.If_ThenLine_ElseLine_ParsesAsInlineGoto
Inputs: "IF A=0 THEN 100 ELSE 200"
Code under test: Parser::parseIf
Expected behavior: Inline IfBlockStmt with thenBody=[GOTO 100], elseBody=[GOTO 200]
*/
TEST(Parser, If_ThenLine_ElseLine_ParsesAsInlineGoto) {
    const char* src =
        "10 IF A=0 THEN 100 ELSE 200\n"
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
    ASSERT_EQ(ib->elseBody.size(), 1u);
    auto* gt1 = dynamic_cast<GotoStmt*>(ib->thenBody[0].get());
    auto* gt2 = dynamic_cast<GotoStmt*>(ib->elseBody[0].get());
    ASSERT_NE(gt1, nullptr);
    ASSERT_NE(gt2, nullptr);
    EXPECT_EQ(gt1->targetLine, 100);
    EXPECT_EQ(gt2->targetLine, 200);
}
