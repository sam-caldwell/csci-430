// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/ArrayAssignStmt.h"
#include "basic_compiler/ast/NumberExpr.h"

using namespace gwbasic;

/***
 * Test: Parser.ArrayAssignmentParses
 * Purpose: Ensure LET A(i)=v parses with index and value expressions.
 * Components Under Test: Parser::parse assignment to array element.
 * Expected Behavior: ArrayAssignStmt has name A, one index 3, and value 4.
 */
TEST(Parser, ArrayAssignmentParses) {
    std::string src = "10 LET A(3) = 4\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto [lines] = p.parseProgram();
    ASSERT_EQ(lines.size(), 1u);
    ASSERT_EQ(lines[0].statements.size(), 1u);
    auto* aa = dynamic_cast<ArrayAssignStmt*>(lines[0].statements[0].get());
    ASSERT_NE(aa, nullptr);
    EXPECT_EQ(aa->name, "A");
    ASSERT_EQ(aa->indices.size(), 1u);
    auto* idx = dynamic_cast<NumberExpr*>(aa->indices[0].get());
    ASSERT_NE(idx, nullptr);
    EXPECT_DOUBLE_EQ(idx->value, 3.0);
    auto* val = dynamic_cast<NumberExpr*>(aa->value.get());
    ASSERT_NE(val, nullptr);
    EXPECT_DOUBLE_EQ(val->value, 4.0);
}
