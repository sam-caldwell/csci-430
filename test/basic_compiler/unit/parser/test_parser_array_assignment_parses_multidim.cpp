// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/ArrayAssignStmt.h"
#include "basic_compiler/ast/NumberExpr.h"

using namespace gwbasic;

/***
 * Test: Parser.ArrayAssignmentParsesMultiDim
 * Purpose: Ensure LET A(i,j)=v parses with two indices.
 * Components Under Test: Parser::parse multidimensional array element assignment.
 * Expected Behavior: ArrayAssignStmt has two indices with expected values.
 */
TEST(Parser, ArrayAssignmentParsesMultiDim) {
    std::string src = "10 LET A(1,2) = 9\n";
    Lexer lex(src); auto toks = lex.tokenize(); Parser p(std::move(toks));
    auto prog = p.parseProgram();
    ASSERT_EQ(prog.lines.size(), 1u);
    auto* aa = dynamic_cast<ArrayAssignStmt*>(prog.lines[0].statements[0].get());
    ASSERT_NE(aa, nullptr);
    ASSERT_EQ(aa->indices.size(), 2u);
    auto* i0 = dynamic_cast<NumberExpr*>(aa->indices[0].get());
    auto* i1 = dynamic_cast<NumberExpr*>(aa->indices[1].get());
    ASSERT_NE(i0, nullptr); ASSERT_NE(i1, nullptr);
    EXPECT_DOUBLE_EQ(i0->value, 1.0);
    EXPECT_DOUBLE_EQ(i1->value, 2.0);
}
