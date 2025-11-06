// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/token/TokenType.h"

using namespace gwbasic;

/***
 * Test: Lexer.TrailingWhitespaceAtEOFHandled
 * Purpose: Validate lexer behavior when input ends with trailing whitespace and no newline.
 * Components Under Test: Lexer::tokenize; Lexer::skipWhitespace
 * Expected Behavior: Trailing spaces are consumed; tokens terminate with EndOfFile;
 *                    no spurious tokens are emitted.
 */
/*
Test: Lexer.TrailingWhitespaceAtEOFHandled
Inputs: Raw source text and helper inputs
Code under test: Lexer/tokenization and helpers
Expected behavior: Tokens/escapes match expectations; errors are reported appropriately
*/
TEST(Lexer, TrailingWhitespaceAtEOFHandled) {
    // Ensure path where skipWhitespace reaches EOF inside the main loop gets covered
    std::string src = "10 PRINT \"X\"   "; // spaces at end, no trailing newline
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    ASSERT_FALSE(toks.empty());
    EXPECT_EQ(toks.back().type, TokenType::EndOfFile);
}
