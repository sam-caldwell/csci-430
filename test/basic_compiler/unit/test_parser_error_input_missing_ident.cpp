// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"

using namespace gwbasic;
/*
 * Test Suite: Parser Error (INPUT missing ident)
 * Purpose: Ensure INPUT requires an identifier.
 * Components Under Test: Parser parseStatement for INPUT.
 * Expected Behavior: Error reported when no identifier follows INPUT.
 */
TEST(Parser, ErrorInputMissingIdentifier) {
    std::string src = "10 INPUT 123\n";
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    EXPECT_THROW({ auto prog = p.parseProgram(); (void)prog; }, ParseError);
}
