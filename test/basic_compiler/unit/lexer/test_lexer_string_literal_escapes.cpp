// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#define private public
#include "basic_compiler/Lexer.h"
#undef private

using namespace gwbasic;

/***
 * Test: Lexer.StringLiteral_DoubledQuotes_And_BackslashesLiteral
 * Purpose: Validate GW-BASIC string rules: doubled quotes inside strings,
 *          and no C-style escape processing (backslashes are literal).
 * Components Under Test: Lexer::stringLiteral
 */
TEST(Lexer, StringLiteral_DoubledQuotes_And_BackslashesLiteral) {
    // Doubled quotes decode to a single quote in the value
    {
        std::string src = R"("He said ""OK""")";
        Lexer L(src);
        Token t = L.stringLiteral();
        ASSERT_EQ(t.type, TokenType::String);
        EXPECT_EQ(t.lexeme, "He said \"OK\"");
    }
    // Backslashes are preserved literally; sequences like \n or \t are two characters
    {
        std::string src = R"("A\nB\tC\qD\\")";
        Lexer L(src);
        Token t = L.stringLiteral();
        ASSERT_EQ(t.type, TokenType::String);
        EXPECT_EQ(t.lexeme, "A\\nB\\tC\\qD\\\\");
    }
}
