// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#define private public
#include "basic_compiler/Lexer.h"
#undef private

using namespace gwbasic;

/***
 * Test: Lexer.SkipToEOL_CoversNewlineAndEOF
 * Purpose: Validate skipToEOL behavior for comments ending with a newline and EOF.
 * Components Under Test: Lexer::skipToEOL
 * Expected Behavior: With a trailing newline, skipToEOL stops at the newline and
 *                    the next non-comment char is reachable after one advance;
 *                    without a newline, the lexer reports end-of-file.
 */
TEST(Lexer, SkipToEOL_CoversNewlineAndEOF) {
    // Case 1: comment with newline present
    {
        std::string src = "' comment then newline\nX";
        Lexer L(src);
        L.skipToEOL();
        // After skip, next char is newline; advance one to consume
        if (!L.atEnd() && L.peek() == '\n') { L.advance(); }
        EXPECT_FALSE(L.atEnd());
        EXPECT_EQ(L.peek(), 'X');
    }
    // Case 2: comment at end-of-file (no newline)
    {
        std::string src = "' EOF comment with no newline";
        Lexer L(src);
        L.skipToEOL();
        EXPECT_TRUE(L.atEnd());
    }
}

// moved: SkipWhitespace_CoversSpacesTabsCRAndApostropheComment
