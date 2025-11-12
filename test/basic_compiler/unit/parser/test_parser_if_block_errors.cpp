// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"

using namespace gwbasic;

/***
 * Test: Parser.IfBlock_Error_MissingEndIf
 * Purpose: Validate that an IF block missing its END IF is rejected.
 * Components Under Test: Lexer::tokenize; Parser::parseProgram
 * Expected Behavior: Parsing throws ParseError when an IF block lacks a closing END IF.
 */
/*
Test: Parser.IfBlock_Error_MissingEndIf
Inputs: BASIC source snippet
Code under test: Parser (BASIC → AST)
Expected behavior: AST structure or parse errors match expectations
*/
TEST(Parser, IfBlock_Error_MissingEndIf) {
    std::string src =
        "10 IF A < 5 THEN\n"
        "20 PRINT 1\n";
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    EXPECT_THROW({ auto prog = p.parseProgram(); (void)prog; }, ParseError);
}
