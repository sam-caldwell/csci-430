// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/lexer/Lexer.h"
#include "basic_compiler/token/TokenType.h"

using namespace gwbasic;

/***
 * Test: Lexer.AmpLiterals_H_O_B_and_Shorthand
 * Purpose: Validate &H, &O, &B, and octal shorthand &NNN are parsed to decimal integer tokens.
 * Components Under Test: Lexer::emitAmpLiteral handling for base-prefixed numbers.
 * Expected Behavior: First token is Integer with expected decimal lexeme for each case.
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
