// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"

using namespace gwbasic;

/*
 * Test Suite: Lexer Errors (unexpected)
 * Purpose: Verify the lexer reports unexpected characters (e.g., '@').
 * Components Under Test: Lexer::tokenize diagnostic path.
 * Expected Behavior: Throw LexError upon encountering an unexpected character.
 *
 * Sad path: unexpected character should raise a LexError.
 */
TEST(Lexer, UnexpectedCharThrows) {
    // '@' is not a recognized token in this lexer.
    std::string src = "10 LET A = @\n";
    Lexer lex(src);
    EXPECT_THROW({ auto toks = lex.tokenize(); (void)toks; }, LexError);
}
