// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include <vector>
#include "basic_compiler/lexer/Lexer.h"
#include "basic_compiler/token/TokenType.h"

using namespace gwbasic;

/***
 * Test: Lexer.AdditionalKeywords_AreRecognized
 * Purpose: Verify that newly added GW-BASIC keywords are classified as
 * keyword tokens by the lexer (not generic identifiers).
 */
TEST(Lexer, AdditionalKeywords_AreRecognized) {
    const char* src =
        "10 FILES\n"
        "20 NAME\n"
        "30 MKDIR\n"
        "40 RMDIR\n"
        "50 SHELL\n"
        "60 ENVIRON\n";
    Lexer lx(src);
    auto toks = lx.tokenize();
    // Extract the non-line-number leading tokens per line
    std::vector<TokenType> types;
    for (size_t i = 0; i + 1 < toks.size(); ++i) {
        if (toks[i].type == TokenType::Integer) {
            types.push_back(toks[i + 1].type);
        }
    }
    ASSERT_GE(types.size(), 6u);
    EXPECT_EQ(types[0], TokenType::KwFiles);
    EXPECT_EQ(types[1], TokenType::KwName);
    EXPECT_EQ(types[2], TokenType::KwMkdir);
    EXPECT_EQ(types[3], TokenType::KwRmdir);
    EXPECT_EQ(types[4], TokenType::KwShell);
    EXPECT_EQ(types[5], TokenType::KwEnviron);
}
