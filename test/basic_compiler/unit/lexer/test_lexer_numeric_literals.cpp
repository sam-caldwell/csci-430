// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/token/TokenType.h"

using namespace gwbasic;

/*
Test: Lexer.ScientificNotation_E_and_D
Inputs: Source strings containing numeric literals with E/D exponents
Code under test: Lexer number literal scanning with exponent support
Expected behavior: Tokens classify as Float and 'D' is normalized to 'E'.
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

/*
Test: Lexer.AmpLiterals_H_O_B_and_Shorthand
Inputs: &H, &O, & (octal shorthand), &B numeric literals
Code under test: Lexer emitAmpLiteral handling for base-prefixed numbers
Expected behavior: Integer tokens with decimal lexeme values.
*/
TEST(Lexer, AmpLiterals_H_O_B_and_Shorthand) {
    struct Case { std::string src; std::string expect; } cases[] = {
        {"&H10\n",  "16"},
        {"&h1f\n",  "31"},
        {"&O10\n",  "8"},
        {"&777\n",  "511"},
        {"&B1010\n","10"},
    };
    for (const auto& cs : cases) {
        std::istringstream iss(cs.src);
        Lexer lex(iss);
        auto toks = lex.tokenize();
        ASSERT_GE(toks.size(), 2u) << cs.src;
        EXPECT_EQ(toks[0].type, TokenType::Integer) << cs.src;
        EXPECT_EQ(toks[0].lexeme, cs.expect) << cs.src;
    }
}

