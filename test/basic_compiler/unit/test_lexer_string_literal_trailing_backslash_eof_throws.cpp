// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#define private public
#include "basic_compiler/Lexer.h"
#undef private

using namespace gwbasic;

/***
 * Test: Lexer.StringLiteral_TrailingBackslashAtEOF_Throws
 * Purpose: Validate behavior when a string literal ends with a backslash at EOF.
 * Components Under Test: Lexer::stringLiteral
 * Expected Behavior: Parsing throws LexError for an unterminated/invalid escape at EOF.
 */
/*
Test: Lexer.StringLiteral_TrailingBackslashAtEOF_Throws
Inputs: Raw source text and helper inputs
Code under test: Lexer/tokenization and helpers
Expected behavior: Tokens/escapes match expectations; errors are reported appropriately
*/
TEST(Lexer, StringLiteral_TrailingBackslashAtEOF_Throws) {
    // A trailing backslash before EOF should result in an unterminated string error
    std::string src = "\"Trailing\\"; // opens quote, backslash, then EOF
    Lexer L(src);
    EXPECT_THROW({ (void)L.stringLiteral(); }, LexError);
}
