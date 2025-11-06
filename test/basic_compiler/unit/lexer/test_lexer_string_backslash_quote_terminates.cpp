// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#define private public
#include "basic_compiler/Lexer.h"
#undef private

using namespace gwbasic;
/*
 * Test: Lexer.BackslashQuote_DoesNotEscape_ClosesString
 * Purpose: Ensure C-style escape for a quote (\") no longer applies.
 *          In GW-BASIC, only doubled quotes ("") encode an embedded quote.
 *          A backslash before a quote does not escape it; the quote closes
 *          the string and the backslash is preserved literally.
 */
TEST(Lexer, BackslashQuote_DoesNotEscape_ClosesString) {
    // Source bytes: '"', 'A', '\\', '"', 'B', '"'
    std::string src = "\"A\\\"B\"";
    Lexer L(src);
    Token t = L.stringLiteral();
    ASSERT_EQ(t.type, TokenType::String);
    // Expect the literal to end before the 'B', with a trailing backslash
    EXPECT_EQ(t.lexeme, std::string("A\\"));
    // Sanity: there should be no embedded quote in the lexeme
    EXPECT_EQ(t.lexeme.find('"'), std::string::npos);
}

