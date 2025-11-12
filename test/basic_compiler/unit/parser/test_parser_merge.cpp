// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"

using namespace gwbasic;

/***
 * Test: Parser.Merge_WithFile_Parses
 * Purpose: Validate parsing of MERGE with a filename operand.
 * Components Under Test: Lexer::tokenize; Parser::parseProgram
 * Expected Behavior: Parses without error and yields one program line containing MERGE.
 */
/*
Test: Parser.Merge_WithFile_Parses
Inputs: BASIC source snippet
Code under test: Parser (BASIC → AST)
Expected behavior: AST structure or parse errors match expectations
*/
TEST(Parser, Merge_WithFile_Parses) {
    std::string src = "10 MERGE \"demo.bas\"\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto [lines] = p.parseProgram();
    ASSERT_EQ(lines.size(), 1u);
}
