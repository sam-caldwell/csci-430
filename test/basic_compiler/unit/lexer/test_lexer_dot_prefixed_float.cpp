// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include <vector>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/token/Token.h"
#include "basic_compiler/token/TokenType.h"

using namespace gwbasic;

/*
Test: Lexer.DotPrefixedFloat_TokenizesAsFloat
Inputs: Source containing .5 and .25 float literals
Code under test: Lexer::tryEmitPrimary + number() starting with '.'
Expected behavior: Emits Float tokens with lexemes ".5" and ".25".
*/
TEST(Lexer, DotPrefixedFloat_TokenizesAsFloat) {
    const std::string src = "10 PRINT .5, .25\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();

    // Find the first two Float tokens
    std::vector<Token> floats;
    for (const auto& t : toks) if (t.type == TokenType::Float) floats.push_back(t);
    ASSERT_GE(floats.size(), 2u);
    EXPECT_EQ(floats[0].lexeme, std::string(".5"));
    EXPECT_EQ(floats[1].lexeme, std::string(".25"));
}

