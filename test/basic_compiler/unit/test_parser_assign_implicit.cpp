// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/AssignStmt.h"

using namespace gwbasic;
/*
 * Test Suite: Parser Assign (implicit)
 * Purpose: Verify assignment without the LET keyword is accepted.
 * Components Under Test: Lexer; Parser parseAssignOrLet.
 * Expected Behavior: Single AssignStmt with identifier A assigned numeric literal.
 */
TEST(Parser, AssignImplicitWithoutLet) {
    std::string src = "10 A = 42\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto [lines] = p.parseProgram();
    ASSERT_EQ(lines.size(), 1u);
    ASSERT_EQ(lines[0].statements.size(), 1u);
    auto* as = dynamic_cast<AssignStmt*>(lines[0].statements[0].get());
    ASSERT_NE(as, nullptr);
    EXPECT_EQ(as->name, "A");
}
