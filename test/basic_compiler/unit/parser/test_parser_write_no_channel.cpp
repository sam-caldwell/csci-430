// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/WriteStmt.h"

using namespace gwbasic;

/***
Test: Parser.WriteParsesWithoutChannel
Inputs: WRITE 1, 2, 3
Code under test: Parser::parseWrite
Expected behavior: Channel remains -1 (stdout), items parsed as expressions.
*/
TEST(Parser, WriteParsesWithoutChannel) {
    const std::string src = "10 WRITE 1, 2, 3\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto [lines] = p.parseProgram();
    ASSERT_EQ(lines.size(), 1u);
    ASSERT_EQ(lines[0].statements.size(), 1u);
    auto* wr = dynamic_cast<WriteStmt*>(lines[0].statements[0].get());
    ASSERT_NE(wr, nullptr);
    EXPECT_EQ(wr->channel, -1);
    EXPECT_EQ(wr->items.size(), 3u);
}

