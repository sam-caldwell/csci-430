// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#define private public
#include "basic_compiler/lexer/Lexer.h"
#undef private

using namespace gwbasic;

/***
 * Test: Lexer.StringLiteral_TrailingBackslash_BeforeClose_IsLiteral
 * Purpose: Ensure a backslash immediately before a closing quote is treated
 *          literally (no C-style escape interpretation).
 */
TEST(Lexer, StringLiteral_TrailingBackslash_BeforeClose_IsLiteral) {
    std::string src = "\"Trailing\\\""; // BASIC: "Trailing\"
    Lexer L(src);
    Token t = L.stringLiteral();
    ASSERT_EQ(t.type, TokenType::String);
    EXPECT_EQ(t.lexeme, "Trailing\\");
}
