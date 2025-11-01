// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/ForStmt.h"
#include "basic_compiler/ast/NumberExpr.h"

using namespace gwbasic;
/*
 * Test Suite: Parser FOR (STEP)
 * Purpose: Verify optional STEP expression is parsed when present.
 * Components Under Test: Parser parseFor, expression parsing.
 * Expected Behavior: ForStmt.step is present and matches parsed expression.
 */
TEST(Parser, ForWithStep) {
    std::string src = "10 FOR I = 1 TO 10 STEP 2 : NEXT I\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto [lines] = p.parseProgram();
    ASSERT_EQ(lines.size(), 1u);
    ASSERT_EQ(lines[0].statements.size(), 1u);
    auto* fs = dynamic_cast<ForStmt*>(lines[0].statements[0].get());
    ASSERT_NE(fs, nullptr);
    ASSERT_NE(fs->step, nullptr);
    auto* stepNum = dynamic_cast<NumberExpr*>(fs->step.get());
    ASSERT_NE(stepNum, nullptr);
    EXPECT_DOUBLE_EQ(stepNum->value, 2.0);
}
