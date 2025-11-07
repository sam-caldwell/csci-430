// (c) 2025 Sam Caldwell. All Rights Reserved.
#define private public
#include <gtest/gtest.h>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"

using namespace gwbasic;

/***
 * Test: ParserHelpers.TryParseOtherKeywords_CommonCases
 * Purpose: Ensure miscellaneous keyword-led statements are recognized by the helper path.
 * Components Under Test: Parser::tryParseOtherKeywords.
 * Expected Behavior: Returns non-null nodes for PRINT, CLEAR, and RANDOMIZE.
 */
TEST(ParserHelpers, TryParseOtherKeywords_CommonCases) {
    // PRINT
    {
        std::string src = "10 PRINT 1\n";
        Lexer lx(src); auto toks = lx.tokenize();
        Parser p(std::move(toks)); p.pos_ = 1;
        auto node = p.tryParseOtherKeywords(p.peek());
        ASSERT_NE(node, nullptr);
    }
    // CLEAR
    {
        std::string src = "10 CLEAR\n";
        Lexer lx(src); auto toks = lx.tokenize();
        Parser p(std::move(toks)); p.pos_ = 1;
        auto node = p.tryParseOtherKeywords(p.peek());
        ASSERT_NE(node, nullptr);
    }
    // RANDOMIZE 5
    {
        std::string src = "10 RANDOMIZE 5\n";
        Lexer lx(src); auto toks = lx.tokenize();
        Parser p(std::move(toks)); p.pos_ = 1;
        auto node = p.tryParseOtherKeywords(p.peek());
        ASSERT_NE(node, nullptr);
    }
}
