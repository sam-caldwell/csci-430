// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#define private public
#include "basic_compiler/Lexer.h"
#undef private

using namespace gwbasic;

/***
 * Test: Lexer.SkipWhitespace_CoversSpacesTabsCRAndApostropheComment
 * Purpose: Validate skipWhitespace handles spaces, tabs, carriage return, and apostrophe comments.
 * Components Under Test: Lexer::skipWhitespace
 * Expected Behavior: Skips leading whitespace and an apostrophe comment up to the newline;
 *                    after consuming the newline, the lexer points at the next token ('A').
 */
TEST(Lexer, SkipWhitespace_CoversSpacesTabsCRAndApostropheComment) {
    std::string src = " \t\r' comment\nA";
    Lexer L(src);
    L.skipWhitespace();
    // After skipping, we should be at newline (end of comment), advance
    if (!L.atEnd() && L.peek() == '\n') { L.advance(); }
    ASSERT_FALSE(L.atEnd());
    EXPECT_EQ(L.peek(), 'A');
}
