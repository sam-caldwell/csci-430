// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"

using namespace gwbasic;
/*
 * Test Suite: Parser Error (IF missing THEN)
 * Purpose: Ensure IF conditions must be followed by THEN.
 * Components Under Test: Parser parseIf.
 * Expected Behavior: Parser reports error when THEN keyword is missing.
 */
/***
Test: Parser.ErrorIfMissingThen
Inputs: BASIC source snippet
Code under test: Parser (BASIC → AST)
Expected behavior: AST structure or parse errors match expectations
*/
TEST(Parser, ErrorIfMissingThen) {
    std::string src = "10 IF A < 10 50\n"; // missing THEN keyword
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    EXPECT_THROW({ auto prog = p.parseProgram(); (void)prog; }, ParseError);
}
