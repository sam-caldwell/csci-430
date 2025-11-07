// (c) 2025 Sam Caldwell. All Rights Reserved.
#define private public
#include <gtest/gtest.h>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"

using namespace gwbasic;

TEST(ParserHelpers, TryParseSpecialIdentifier_Screen) {
    std::string src = "10 SCREEN 1\n";
    Lexer lx(src); auto toks = lx.tokenize();
    Parser p(std::move(toks));
    // Advance to the 'SCREEN' identifier token
    for (size_t i = 0; i < p.tokens_.size(); ++i) {
        if (p.tokens_[i].type == TokenType::Identifier && p.tokens_[i].lexeme == "SCREEN") { p.pos_ = i; break; }
    }
    auto node = p.tryParseSpecialIdentifierStatement(p.peek());
    ASSERT_NE(node, nullptr);
}

