// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/WriteStmt.h"

using namespace gwbasic;

/***
 * Test: Parser.WriteParsesWithChannel
 * Purpose: Ensure WRITE #n, expr, expr parses with channel and items.
 */
TEST(Parser, WriteParsesWithChannel) {
    std::string src = "10 WRITE #2, 1, 2+3\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto [lines] = p.parseProgram();
    ASSERT_EQ(lines.size(), 1u);
    ASSERT_EQ(lines[0].statements.size(), 1u);
    auto* wr = dynamic_cast<WriteStmt*>(lines[0].statements[0].get());
    ASSERT_NE(wr, nullptr);
    EXPECT_EQ(wr->channel, 2);
    ASSERT_EQ(wr->items.size(), 2u);
}

