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
/*
Test: Lexer.UnexpectedCharThrows
Inputs: Raw source text and helper inputs
Code under test: Lexer/tokenization and helpers
Expected behavior: Tokens/escapes match expectations; errors are reported appropriately
*/
TEST(Lexer, UnexpectedCharThrows) {
    // '@' is not a recognized token in this lexer.
    std::string src = "10 LET A = @\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    EXPECT_THROW({ auto toks = lex.tokenize(); (void)toks; }, LexError);
}
