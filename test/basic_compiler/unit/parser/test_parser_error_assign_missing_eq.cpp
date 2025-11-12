// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"

using namespace gwbasic;
/*
 * Test Suite: Parser Error (missing '=')
 * Purpose: Ensure the parser reports an error when '=' is missing in the assignment.
 * Components Under Test: Parser parseAssignOrLet error path.
 * Expected Behavior: Parsing throws/indicates error for invalid assignment.
 */
/***
Test: Parser.ErrorAssignMissingEquals
Inputs: BASIC source snippet
Code under test: Parser (BASIC → AST)
Expected behavior: AST structure or parse errors match expectations
*/
TEST(Parser, ErrorAssignMissingEquals) {
    std::string src = "10 LET A 1\n"; // missing '='
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    EXPECT_THROW({ auto prog = p.parseProgram(); (void)prog; }, ParseError);
}
