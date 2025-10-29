// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"

using namespace gwbasic;
/*
 * Test Suite: Parser Error (IF missing line)
 * Purpose: Ensure THEN must be followed by a line number.
 * Components Under Test: Parser parseIf/parseComparison.
 * Expected Behavior: Error reported when IF lacks a THEN target line.
 */
TEST(Parser, ErrorIfMissingLineNumber) {
    std::string src = "10 IF A > 1 THEN PRINT 5\n"; // not a line number
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    EXPECT_THROW({ auto prog = p.parseProgram(); (void)prog; }, ParseError);
}
