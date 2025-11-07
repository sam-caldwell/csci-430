// (c) 2025 Sam Caldwell. All Rights Reserved.
#define private public
#include <gtest/gtest.h>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"

using namespace gwbasic;

/***
Test: ParserHelpers.TryParseSpecialIdentifier_Circle
Purpose: Ensure special-identifier handler recognizes and parses CIRCLE statement.
Components Under Test: Parser::tryParseSpecialIdentifierStatement.
Expected Behavior: Returns non-null node when positioned at 'CIRCLE'.
*/
TEST(ParserHelpers, TryParseSpecialIdentifier_Circle) {
    std::string src = "10 CIRCLE (10,10),5\n";
    Lexer lx(src); auto toks = lx.tokenize();
    Parser p(std::move(toks));
    for (size_t i = 0; i < p.tokens_.size(); ++i) {
        if (p.tokens_[i].type == TokenType::Identifier && p.tokens_[i].lexeme == "CIRCLE") { p.pos_ = i; break; }
    }
    auto node = p.tryParseSpecialIdentifierStatement(p.peek());
    ASSERT_NE(node, nullptr);
}
