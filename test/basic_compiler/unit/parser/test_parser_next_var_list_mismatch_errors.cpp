// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/parser/ParseError.h"

/***
 * Test: Parser.NextVarList_MismatchErrors
 * Purpose: Ensure mismatched NEXT var-list order causes a parse error.
 * Components Under Test: Parser::parseProgram FOR/NEXT matching.
 * Expected Behavior: Throws ParseError when NEXT order does not match nesting.
 */
TEST(Parser, NextVarList_MismatchErrors) {
    const std::string src =
        "10 FOR I = 1 TO 1\n"
        "20 FOR J = 1 TO 1\n"
        "30 NEXT I, J\n"; // wrong order; first NEXT must match J
    std::istringstream in(src);
    gwbasic::Lexer lx(in);
    auto toks = lx.tokenize();
    gwbasic::Parser p(std::move(toks));
    EXPECT_THROW({ (void)p.parseProgram(); }, gwbasic::ParseError);
}
