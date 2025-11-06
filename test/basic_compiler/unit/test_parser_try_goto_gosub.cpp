// (c) 2025 Sam Caldwell. All Rights Reserved.
#define private public
#include <gtest/gtest.h>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"

using namespace gwbasic;

TEST(ParserHelpers, TryParseGotoGosub) {
    {
        std::string src = "10 GOTO 100\n";
        Lexer lx(src); auto toks = lx.tokenize();
        Parser p(std::move(toks)); p.pos_ = 1;
        auto node = p.tryParseGotoGosub(p.peek());
        ASSERT_NE(node, nullptr);
    }
    {
        std::string src = "10 GOSUB 200\n";
        Lexer lx(src); auto toks = lx.tokenize();
        Parser p(std::move(toks)); p.pos_ = 1;
        auto node = p.tryParseGotoGosub(p.peek());
        ASSERT_NE(node, nullptr);
    }
}

