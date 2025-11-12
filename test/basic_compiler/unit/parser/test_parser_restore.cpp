// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/RestoreStmt.h"

using namespace gwbasic;

/***
 * Test: Parser.RestoreParses
 * Purpose: Ensure RESTORE parses into RestoreStmt.
 */
/*
Test: Parser.RestoreParses
Inputs: BASIC source snippet
Code under test: Parser (BASIC → AST)
Expected behavior: AST structure or parse errors match expectations
*/
TEST(Parser, RestoreParses) {
    std::string src = "10 RESTORE\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto [lines] = p.parseProgram();
    ASSERT_EQ(lines.size(), 1u);
    auto* rs = dynamic_cast<RestoreStmt*>(lines[0].statements[0].get());
    ASSERT_NE(rs, nullptr);
}
