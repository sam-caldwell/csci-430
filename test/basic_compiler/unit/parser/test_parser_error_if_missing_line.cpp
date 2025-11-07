// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/IfBlockStmt.h"

using namespace gwbasic;
/*
 * Test Suite: Parser Error (IF missing line)
 * Purpose: Previously asserted THEN must be a line number. With inline IF
 *          support, THEN may be followed by a statement list; update to
 *          assert that form parses as an inline IfBlockStmt.
 * Components Under Test: Parser parseIf/parseComparison.
 * Expected Behavior: Error reported when IF lacks a THEN target line.
 */
/***
Test: Parser.ErrorIfMissingLineNumber
Inputs: BASIC source snippet
Code under test: Parser (BASIC → AST)
Expected behavior: AST structure or parse errors match expectations
*/
TEST(Parser, ErrorIfMissingLineNumber) {
    std::string src = "10 IF A > 1 THEN PRINT 5\n"; // inline THEN statement-list
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();
    ASSERT_EQ(prog.lines.size(), 1u);
    ASSERT_EQ(prog.lines[0].statements.size(), 1u);
    auto* ib = dynamic_cast<IfBlockStmt*>(prog.lines[0].statements[0].get());
    ASSERT_NE(ib, nullptr);
    EXPECT_TRUE(ib->inlineEnd);
    ASSERT_EQ(ib->thenBody.size(), 1u);
}
