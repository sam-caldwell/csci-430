// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/parser/ParseError.h"

using namespace gwbasic;

/***
Test: ParserPrimary.ErrorOnUnexpectedToken
Purpose: Ensure parser reports an error when encountering an unexpected token in primary position.
Components Under Test: Parser::parseProgram; parsePrimary error path.
Expected Behavior: Throws ParseError when source contains an unexpected ')'.
*/
TEST(ParserPrimary, ErrorOnUnexpectedToken) {
    const std::string src = "10 PRINT )\n"; // unexpected
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    EXPECT_THROW({ (void)p.parseProgram(); }, ParseError);
}
