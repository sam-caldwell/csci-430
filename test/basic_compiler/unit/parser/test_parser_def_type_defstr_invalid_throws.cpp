// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/parser/ParseError.h"

using namespace gwbasic;

/***
Test: Parser.DEFSTR_InvalidDigit_Throws
Inputs: DEFSTR 1
Code under test: Parser::parseProgram → parseDefType
Expected behavior: Throws ParseError (digit is not a letter A-Z)
*/
TEST(Parser, DEFSTR_InvalidDigit_Throws) {
    const char* src = "10 DEFSTR 1\n";
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    EXPECT_THROW({ (void)p.parseProgram(); }, ParseError);
}

