// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/WhileStmt.h"
#include "basic_compiler/ast/PrintStmt.h"

using namespace gwbasic;

/***
 * Test: Parser.WhileInlineBody
 * Purpose: Validate parsing of a WHILE loop with an inline body terminated by WEND on the same line.
 * Components Under Test: Lexer::tokenize; Parser::parseProgram; AST WhileStmt
 * Expected Behavior: Produces one WhileStmt whose body contains a single PrintStmt.
 */
/*
Test: Parser.WhileInlineBody
Inputs: BASIC source snippet
Code under test: Parser (BASIC → AST)
Expected behavior: AST structure or parse errors match expectations
*/
TEST(Parser, WhileInlineBody) {
    std::string src = "10 WHILE I < 3 : PRINT I : WEND\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto [lines] = p.parseProgram();
    ASSERT_EQ(lines.size(), 1u);
    ASSERT_EQ(lines[0].statements.size(), 1u);
    auto* ws = dynamic_cast<WhileStmt*>(lines[0].statements[0].get());
    ASSERT_NE(ws, nullptr);
    ASSERT_EQ(ws->body.size(), 1u);
    ASSERT_NE(dynamic_cast<PrintStmt*>(ws->body[0].get()), nullptr);
}
