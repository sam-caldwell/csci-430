// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"

using namespace gwbasic;

/***
 * Test: Parser.Chain_MissingFilename_Errors
 * Purpose: Validate that CHAIN without a filename is rejected during parsing.
 * Components Under Test: Lexer::tokenize; Parser::parseProgram
 * Expected Behavior: Parsing throws ParseError when CHAIN lacks a filename.
 */
TEST(Parser, Chain_MissingFilename_Errors) {
    std::string src = "10 CHAIN\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    EXPECT_THROW({ (void)p.parseProgram(); }, ParseError);
}
