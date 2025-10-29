// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"

using namespace gwbasic;
/*
 * Test Suite: Lexer Errors (unterminated string)
 * Purpose: Ensure missing closing quote in string literal is diagnosed.
 * Components Under Test: Lexer::stringLiteral and error propagation.
 * Expected Behavior: Throw LexError for unterminated string.
 *
 * Sad path: unterminated string literal should raise a LexError.
 */
TEST(Lexer, UnterminatedStringThrows) {
    std::string src = "10 PRINT \"Hello\n"; // missing closing quote
    Lexer lex(src);
    EXPECT_THROW({ auto toks = lex.tokenize(); (void)toks; }, LexError);
}
