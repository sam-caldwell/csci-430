// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "basic_compiler/LexerAccessorForTests.h"

using namespace gwbasic;

/***
Test: Lexer.EmitAmpLiteral_HappyPaths
Purpose: Directly exercise emitAmpLiteral() via the test accessor to validate
         correct base handling, token shape, and cursor/BOL effects.
Security checks: Ensures only known designators (H/O/B or digit shorthand) are
                 accepted, parsing stops at first non-digit for the radix, and
                 token positions are assigned from provided line/col.
*/
TEST(Lexer, EmitAmpLiteral_HappyPaths) {
    struct Case { std::string src; std::string expect; } cases[] = {
        {"&H10",   "16"},
        {"&h1f",   "31"},
        {"&O10",   "8"},
        {"&777",   "511"},
        {"&B1010", "10"},
        // Stops at first invalid digit for the base ("G" not consumed)
        {"&H1G",   "1"},
    };

    for (const auto& cs : cases) {
        Lexer lx(cs.src);
        std::vector<Token> out;
        // Precondition: peek() should be '&'
        ASSERT_EQ(LexerAccessorForTests::peek(lx), '&');
        // Assign a synthetic position to verify token carries it
        LexerAccessorForTests::emitAmpLiteral(lx, out, /*line*/42, /*col*/7);
        ASSERT_EQ(out.size(), 1u) << cs.src;
        const Token& t = out[0];
        EXPECT_EQ(t.type, TokenType::Integer) << cs.src;
        EXPECT_EQ(t.lexeme, cs.expect) << cs.src;
        EXPECT_EQ(t.line, 42);
        EXPECT_EQ(t.col, 7);
        // After emit, BOL should be false and cursor at next char (or EOF)
        EXPECT_FALSE(LexerAccessorForTests::bol(lx));
        // For &H1G case, ensure we did not consume 'G'
        if (cs.src == "&H1G") {
            EXPECT_EQ(LexerAccessorForTests::peek(lx), 'G');
        }
    }
}

/***
Test: Lexer.EmitAmpLiteral_SadPaths
Purpose: Validate security/error handling on invalid shapes.
Cases:
 - "&" (EOF after ampersand) → Unexpected '&' error
 - "&H" (no digits) → Invalid & literal
 - "&O8" (invalid digit for octal) → Invalid & literal
 - "&B2" (invalid digit for binary) → Invalid & literal
 - "&9" (invalid shorthand digit) → Invalid & literal
 - "&Q1" (unknown designator) → Unexpected '&' error
*/
TEST(Lexer, EmitAmpLiteral_SadPaths) {
    auto expect_throw = [](std::string s) {
        Lexer lx(std::move(s));
        std::vector<Token> out;
        ASSERT_EQ(LexerAccessorForTests::peek(lx), '&');
        EXPECT_THROW({ LexerAccessorForTests::emitAmpLiteral(lx, out, 1, 1); }, LexError);
    };

    expect_throw("&");      // EOF after '&'
    expect_throw("&H");     // Missing digits
    expect_throw("&O8");    // Invalid octal digit
    expect_throw("&B2");    // Invalid binary digit
    expect_throw("&9");     // Invalid shorthand (non-octal)
    expect_throw("&Q1");    // Unknown base designator
}
