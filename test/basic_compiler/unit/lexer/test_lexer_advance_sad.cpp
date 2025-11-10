// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/LexerAccessorForTests.h"

using namespace gwbasic;

/***
Test: Lexer.Advance_SadPath_AtEnd
Inputs: Source string "A"
Code under test: char Lexer::advance() via LexerAccessorForTests
Expected behavior: After consuming the only character, subsequent advance()
                   returns '\0' and leaves line/col/bol unchanged.
*/
TEST(Lexer, Advance_SadPath_AtEnd) {
    Lexer lx(std::string("A"));

    // First advance consumes 'A'
    ASSERT_EQ(LexerAccessorForTests::advance(lx), 'A');
    int line = LexerAccessorForTests::line(lx);
    int col  = LexerAccessorForTests::col(lx);
    bool bol = LexerAccessorForTests::bol(lx);
    ASSERT_TRUE(LexerAccessorForTests::atEnd(lx));

    // Second advance at end returns NUL and does not mutate state
    EXPECT_EQ(LexerAccessorForTests::advance(lx), '\0');
    EXPECT_EQ(LexerAccessorForTests::line(lx), line);
    EXPECT_EQ(LexerAccessorForTests::col(lx), col);
    EXPECT_EQ(LexerAccessorForTests::bol(lx), bol);
}
