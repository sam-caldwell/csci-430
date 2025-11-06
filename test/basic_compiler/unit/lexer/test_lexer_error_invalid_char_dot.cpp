// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/LexError.h"

using namespace gwbasic;
/*
 * Test Suite: Lexer Errors (.)
 * Purpose: Verify a standalone '.' is rejected (not part of a float).
 * Components Under Test: Lexer::tokenize error handling for punctuation.
 * Expected Behavior: Throw LexError when encountering '.' not forming a float.
 *
 * Sad path: a standalone '.' is not a valid token and should throw.
 */
/*
Test: Lexer.InvalidCharDotThrows
Inputs: Raw source text and helper inputs
Code under test: Lexer/tokenization and helpers
Expected behavior: Tokens/escapes match expectations; errors are reported appropriately
*/
TEST(Lexer, InvalidCharDotThrows) {
    std::string src = "10 LET A = .\n"; // '.' is not recognized (outside a number)
    std::istringstream iss(src);
    Lexer lex(iss);
    EXPECT_THROW({ auto toks = lex.tokenize(); (void)toks; }, LexError);
}
