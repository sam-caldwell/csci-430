// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/GotoStmt.h"

using namespace gwbasic;
/*
 * Test Suite: Parser GOTO
 * Purpose: Validate parsing of GOTO with numeric target.
 * Components Under Test: Parser parseStatement for GOTO.
 * Expected Behavior: AST contains GotoStmt with expected line target.
 */
TEST(Parser, Goto) {
    std::string src = "10 GOTO 200\n";
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto [lines] = p.parseProgram();
    ASSERT_EQ(lines.size(), 1u);
    auto* gs = dynamic_cast<GotoStmt*>(lines[0].statements[0].get());
    ASSERT_NE(gs, nullptr);
    EXPECT_EQ(gs->targetLine, 200);
}
