// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/IfStmt.h"
#include "basic_compiler/ast/BinaryExpr.h"

using namespace gwbasic;
/*
 * Test Suite: Parser IF/THEN
 * Purpose: Verify IF comparison and THEN target line parsing.
 * Components Under Test: Parser parseIf/parseComparison.
 * Expected Behavior: IfStmt with BinaryExpr comparison and correct target line.
 */
TEST(Parser, IfThenComparison) {
    std::string src = "10 IF A <= 10 THEN 50\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto [lines] = p.parseProgram();
    ASSERT_EQ(lines.size(), 1u);
    ASSERT_EQ(lines[0].statements.size(), 1u);
    auto* is = dynamic_cast<IfStmt*>(lines[0].statements[0].get());
    ASSERT_NE(is, nullptr);
    EXPECT_EQ(is->targetLine, 50);
    auto* cmp = dynamic_cast<BinaryExpr*>(is->cond.get());
    ASSERT_NE(cmp, nullptr);
}
