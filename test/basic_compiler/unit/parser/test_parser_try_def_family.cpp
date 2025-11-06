// (c) 2025 Sam Caldwell. All Rights Reserved.
#define private public
#include <gtest/gtest.h>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"

using namespace gwbasic;

TEST(ParserHelpers, TryParseDefFamily_Variants) {
    // DEF SEG
    {
        std::string src = "10 DEF SEG = 0\n";
        Lexer lx(src); auto toks = lx.tokenize();
        Parser p(std::move(toks)); p.pos_ = 1;
        auto node = p.tryParseDefFamily(p.peek());
        ASSERT_NE(node, nullptr);
    }
    // DEF USR
    {
        std::string src = "10 DEF USR0=0\n";
        Lexer lx(src); auto toks = lx.tokenize();
        Parser p(std::move(toks)); p.pos_ = 1;
        auto node = p.tryParseDefFamily(p.peek());
        ASSERT_NE(node, nullptr);
    }
    // DEF FN
    {
        std::string src = "10 DEF FNX(X)=X\n";
        Lexer lx(src); auto toks = lx.tokenize();
        Parser p(std::move(toks)); p.pos_ = 1;
        auto node = p.tryParseDefFamily(p.peek());
        ASSERT_NE(node, nullptr);
    }
}

