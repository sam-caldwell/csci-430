// (c) 2025 Sam Caldwell. All Rights Reserved.
#include <gtest/gtest.h>
#include "basic_compiler/Lexer.h"

using namespace gwbasic;
/*
 * Test Suite: Lexer Basic Tokens
 * Purpose: Sanity-check tokenization of a small program and ensure common
 *          token kinds and at least one NewLine are produced.
 * Components Under Test: Lexer::tokenize, TokenType classification.
 * Expected Behavior: Sequence includes integer, identifiers, operators,
 *          keywords, and a NewLine token.
 */
TEST(Lexer, BasicTokens) {
    std::string src = R"(10 LET A = 1+2*3
20 PRINT "Hello"
)";
    Lexer lex(src);
    auto toks = lex.tokenize();
    ASSERT_GE(toks.size(), 12u);
    EXPECT_EQ(toks[0].type, TokenType::Integer);
    EXPECT_EQ(toks[1].type, TokenType::KwLet);
    EXPECT_EQ(toks[2].type, TokenType::Identifier);
    EXPECT_EQ(toks[3].type, TokenType::Assign);
    EXPECT_EQ(toks[4].type, TokenType::Integer);
    EXPECT_EQ(toks[5].type, TokenType::Plus);
    EXPECT_EQ(toks[6].type, TokenType::Integer);
    EXPECT_EQ(toks[7].type, TokenType::Star);
    EXPECT_EQ(toks[8].type, TokenType::Integer);
    bool sawNewline = false;
    for (auto &t: toks) if (t.type == TokenType::NewLine) { sawNewline = true; break; }
    EXPECT_TRUE(sawNewline);
}
