// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/WhileStmt.h"
#include "basic_compiler/ast/PrintStmt.h"

using namespace gwbasic;

/***
 * Test: Parser.WhileMultilineBody
 * Purpose: Validate parsing of a multiline WHILE loop body terminated by WEND on a later line.
 * Components Under Test: Lexer::tokenize; Parser::parseProgram; AST WhileStmt
 * Expected Behavior: Produces one WhileStmt with inlineWend=false and a body containing a single PrintStmt.
 */
/*
Test: Parser.WhileMultilineBody
Inputs: BASIC source snippet
Code under test: Parser (BASIC → AST)
Expected behavior: AST structure or parse errors match expectations
*/
TEST(Parser, WhileMultilineBody) {
    std::string src =
        "10 WHILE I < 3\n"
        "20 PRINT I\n"
        "30 WEND\n";
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto [lines] = p.parseProgram();
    ASSERT_EQ(lines.size(), 1u);
    ASSERT_EQ(lines[0].statements.size(), 1u);
    auto* ws = dynamic_cast<WhileStmt*>(lines[0].statements[0].get());
    ASSERT_NE(ws, nullptr);
    EXPECT_FALSE(ws->inlineWend);
    ASSERT_EQ(ws->body.size(), 1u);
    ASSERT_NE(dynamic_cast<PrintStmt*>(ws->body[0].get()), nullptr);
}
