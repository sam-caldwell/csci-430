// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"

using namespace gwbasic;
/*
 * Test Suite: Lexer Errors (!)
 * Purpose: Verify the lexer rejects unsupported punctuation '!' with a
 *          diagnostic.
 * Components Under Test: Lexer::tokenize error handling.
 * Expected Behavior: Throw LexError when encountering '!'.
 *
 * Sad path: '!' is not a valid token and should throw.
 */
TEST(Lexer, InvalidCharBangThrows) {
    std::string src = "10 LET A = !\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    EXPECT_THROW({ auto toks = lex.tokenize(); (void)toks; }, LexError);
}
