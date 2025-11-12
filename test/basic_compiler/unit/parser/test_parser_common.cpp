// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/CommonStmt.h"

using namespace gwbasic;

/*
 * Test Suite: Parser COMMON
 * Purpose: Verify COMMON var[,var...] parses correctly.
 */
/***
Test: Parser.Common_DeclList_Parses
Purpose: Verify COMMON var[,var...] parses correctly.
Components Under Test: Parser parseStatement for COMMON.
Expected Behavior: CommonStmt produced with expected names list.
*/
TEST(Parser, Common_DeclList_Parses) {
    std::string src = "10 COMMON A, B, C\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto [lines] = p.parseProgram();
    ASSERT_EQ(lines.size(), 1u);
    auto* cs = dynamic_cast<CommonStmt*>(lines[0].statements[0].get());
    ASSERT_NE(cs, nullptr);
    ASSERT_EQ(cs->names.size(), 3u);
    EXPECT_EQ(cs->names[0], "A");
    EXPECT_EQ(cs->names[1], "B");
    EXPECT_EQ(cs->names[2], "C");
}
