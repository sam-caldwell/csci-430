// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/token/TokenType.h"

using namespace gwbasic;

/***
 * Test: Lexer.ScientificNotation_E_and_D
 * Purpose: Ensure numeric literals support exponents with 'E' and normalize 'D' to 'E'.
 * Components Under Test: Lexer::number() exponent parsing.
 * Expected Behavior: Tokens are Float with lexemes "1E2" and "1E-3".
 */
TEST(Lexer, ScientificNotation_E_and_D) {
    {
        std::string src = "1E2\n";
        std::istringstream iss(src);
        Lexer lex(iss);
        auto toks = lex.tokenize();
        ASSERT_GE(toks.size(), 2u);
        EXPECT_EQ(toks[0].type, TokenType::Float);
        EXPECT_EQ(toks[0].lexeme, "1E2");
    }
    {
        std::string src = "1d-3\n";
        std::istringstream iss(src);
        Lexer lex(iss);
        auto toks = lex.tokenize();
        ASSERT_GE(toks.size(), 2u);
        EXPECT_EQ(toks[0].type, TokenType::Float);
        // 'd' normalized to 'E'
        EXPECT_EQ(toks[0].lexeme, "1E-3");
    }
}
