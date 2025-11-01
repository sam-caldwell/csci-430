// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include <sstream>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/GosubStmt.h"
#include "basic_compiler/ast/ReturnStmt.h"

using namespace gwbasic;
/*
 * Test Suite: Parser GOSUB/RETURN
 * Purpose: Ensure GOSUB targets and RETURN statements parse correctly.
 * Components Under Test: Parser parseStatement for GOSUB/RETURN.
 * Expected Behavior: AST contains GosubStmt and corresponding ReturnStmt.
 */
TEST(Parser, GosubAndReturn) {
    std::string src = "10 GOSUB 300\n20 RETURN\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto [lines] = p.parseProgram();
    ASSERT_EQ(lines.size(), 2u);
    auto* gs = dynamic_cast<GosubStmt*>(lines[0].statements[0].get());
    ASSERT_NE(gs, nullptr);
    EXPECT_EQ(gs->targetLine, 300);
    auto* rs = dynamic_cast<ReturnStmt*>(lines[1].statements[0].get());
    ASSERT_NE(rs, nullptr);
}
