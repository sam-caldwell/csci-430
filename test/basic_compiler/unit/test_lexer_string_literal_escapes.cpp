// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#define private public
#include "basic_compiler/Lexer.h"
#undef private

using namespace gwbasic;

/***
 * Test: Lexer.StringLiteral_KnownAndUnknownEscapes
 * Purpose: Validate stringLiteral parsing of known escapes and behavior for unknown escapes.
 * Components Under Test: Lexer::stringLiteral
 * Expected Behavior: Recognizes and materializes \n, \t, \" and \\ escapes; for an
 *                    unknown escape (e.g., \q), emits the literal character ("q").
 */
/*
Test: Lexer.StringLiteral_KnownAndUnknownEscapes
Inputs: Raw source text and helper inputs
Code under test: Lexer/tokenization and helpers
Expected behavior: Tokens/escapes match expectations; errors are reported appropriately
*/
TEST(Lexer, StringLiteral_KnownAndUnknownEscapes) {
    // Known escapes: \n, \t, quote, backslash
    {
        std::string src = R"("A\nB\tC\"D\\E")";
        Lexer L(src);
        Token t = L.stringLiteral();
        ASSERT_EQ(t.type, TokenType::String);
        EXPECT_NE(t.lexeme.find("A\nB\tC\"D\\E"), std::string::npos);
    }
    // Unknown escape should keep the character as-is
    {
        std::string src = R"("Z\qY")";
        Lexer L(src);
        Token t = L.stringLiteral();
        ASSERT_EQ(t.type, TokenType::String);
        EXPECT_EQ(t.lexeme, "ZqY");
    }
}
