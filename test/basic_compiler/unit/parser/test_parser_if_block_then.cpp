// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/IfBlockStmt.h"
#include "basic_compiler/ast/PrintStmt.h"

using namespace gwbasic;

/***
 * Test: Parser.IfBlock_ThenOnly_Multiline
 * Purpose: Validate parsing of a multiline IF ... THEN block without an ELSE.
 * Components Under Test: Lexer::tokenize; Parser::parseProgram; AST IfBlockStmt
 * Expected Behavior: Produces one IfBlockStmt with an empty elseBody and a thenBody
 *                    containing a single PrintStmt.
 */
/*
Test: Parser.IfBlock_ThenOnly_Multiline
Inputs: BASIC source snippet
Code under test: Parser (BASIC → AST)
Expected behavior: AST structure or parse errors match expectations
*/
TEST(Parser, IfBlock_ThenOnly_Multiline) {
    std::string src =
        "10 IF A < 5 THEN\n"
        "20 PRINT 1\n"
        "30 END IF\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto [lines] = p.parseProgram();
    ASSERT_EQ(lines.size(), 1u);
    ASSERT_EQ(lines[0].statements.size(), 1u);
    auto* ib = dynamic_cast<IfBlockStmt*>(lines[0].statements[0].get());
    ASSERT_NE(ib, nullptr);
    ASSERT_EQ(ib->elseBody.size(), 0u);
    ASSERT_EQ(ib->thenBody.size(), 1u);
    ASSERT_NE(dynamic_cast<PrintStmt*>(ib->thenBody[0].get()), nullptr);
}
