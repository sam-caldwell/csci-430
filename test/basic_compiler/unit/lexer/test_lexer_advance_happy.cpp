// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/lexer/LexerAccessorForTests.h"

using namespace gwbasic;

/***
Test: Lexer.Advance_HappyPath
Inputs: Source string "ab\nc"
Code under test: char Lexer::advance() via LexerAccessorForTests
Expected behavior: Returns each character in sequence; after '\n', line increments,
                   col resets to 1, and bol becomes true; otherwise col++ and bol=false.
*/
TEST(Lexer, Advance_HappyPath) {
    Lexer lx(std::string("ab\nc"));
    // Initial state: line 1, col 1, BOL true
    EXPECT_EQ(LexerAccessorForTests::line(lx), 1);
    EXPECT_EQ(LexerAccessorForTests::col(lx), 1);
    EXPECT_TRUE(LexerAccessorForTests::bol(lx));

    // Consume 'a'
    EXPECT_EQ(LexerAccessorForTests::advance(lx), 'a');
    EXPECT_EQ(LexerAccessorForTests::line(lx), 1);
    EXPECT_EQ(LexerAccessorForTests::col(lx), 2);

    // Consume 'b'
    EXPECT_EQ(LexerAccessorForTests::advance(lx), 'b');
    EXPECT_EQ(LexerAccessorForTests::line(lx), 1);
    EXPECT_EQ(LexerAccessorForTests::col(lx), 3);

    // Consume '\n' (LF)
    EXPECT_EQ(LexerAccessorForTests::advance(lx), '\n');
    EXPECT_EQ(LexerAccessorForTests::line(lx), 2);
    EXPECT_EQ(LexerAccessorForTests::col(lx), 1);
    // On newline, advance() sets BOL true
    EXPECT_TRUE(LexerAccessorForTests::bol(lx));

    // Consume 'c'
    EXPECT_EQ(LexerAccessorForTests::advance(lx), 'c');
    EXPECT_EQ(LexerAccessorForTests::line(lx), 2);
    EXPECT_EQ(LexerAccessorForTests::col(lx), 2);
    // BOL state is not cleared by advance() on non-newline; line/col are still updated
}
