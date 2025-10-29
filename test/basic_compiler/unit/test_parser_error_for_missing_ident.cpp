// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"

using namespace gwbasic;
/*
 * Test Suite: Parser Error (FOR missing ident)
 * Purpose: Ensure parser requires an induction variable name in FOR.
 * Components Under Test: Parser parseFor.
 * Expected Behavior: Parsing indicates an error when identifier is absent.
 */
TEST(Parser, ErrorForMissingIdentifier) {
    std::string src = "10 FOR = 1 TO 2 : NEXT\n"; // no variable name after FOR
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    EXPECT_THROW({ auto prog = p.parseProgram(); (void)prog; }, ParseError);
}
