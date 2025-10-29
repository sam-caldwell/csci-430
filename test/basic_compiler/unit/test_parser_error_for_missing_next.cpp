// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"

using namespace gwbasic;
/*
 * Test Suite: Parser Error (FOR missing NEXT)
 * Purpose: Ensure inline FOR body requires a trailing NEXT in same line.
 * Components Under Test: Parser parseFor.
 * Expected Behavior: Error is reported if NEXT is missing.
 */
TEST(Parser, ErrorForMissingNextSameLine) {
    std::string src = "10 FOR I = 1 TO 3 : PRINT I\n"; // missing NEXT on same line
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    EXPECT_THROW({ auto prog = p.parseProgram(); (void)prog; }, ParseError);
}
