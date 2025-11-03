// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/ChainStmt.h"

using namespace gwbasic;

/***
 * Test: Parser.Chain_FileLineAll_Parses
 * Purpose: Validate parsing of CHAIN with filename, target line, and ALL flag.
 * Components Under Test: Lexer::tokenize; Parser::parseProgram; AST ChainStmt
 * Expected Behavior: Produces a ChainStmt with filename "PROG", targetLine = 200,
 *                    and ALL set to true.
 */
TEST(Parser, Chain_FileLineAll_Parses) {
    std::string src = "10 CHAIN \"PROG\", 200, ALL\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto [lines] = p.parseProgram();
    ASSERT_EQ(lines.size(), 1u);
    auto* ch = dynamic_cast<ChainStmt*>(lines[0].statements[0].get());
    ASSERT_NE(ch, nullptr);
    ASSERT_TRUE(ch->filename.has_value());
    EXPECT_EQ(*ch->filename, "PROG");
    ASSERT_TRUE(ch->targetLine.has_value());
    EXPECT_EQ(*ch->targetLine, 200);
    EXPECT_TRUE(ch->all);
}
