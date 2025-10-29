// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"

using namespace gwbasic;
/*
 * Test Suite: Parser Error (missing '=')
 * Purpose: Ensure the parser reports an error when '=' is missing in assignment.
 * Components Under Test: Parser parseAssignOrLet error path.
 * Expected Behavior: Parsing throws/indicates error for invalid assignment.
 */
TEST(Parser, ErrorAssignMissingEquals) {
    std::string src = "10 LET A 1\n"; // missing '='
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    EXPECT_THROW({ auto prog = p.parseProgram(); (void)prog; }, ParseError);
}
