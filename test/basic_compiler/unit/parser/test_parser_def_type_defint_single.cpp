// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/DefTypeStmt.h"

using namespace gwbasic;

/***
Test: Parser.DEFINT_SingleLetter
Inputs: DEFINT A
Code under test: Parser::parseProgram → parseDefType
Expected behavior: One range [A,A], kind=Int
*/
TEST(Parser, DEFINT_SingleLetter) {
    const char* src = "10 DEFINT A\n";
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();
    ASSERT_EQ(prog.lines.size(), 1u);
    ASSERT_EQ(prog.lines[0].statements.size(), 1u);
    auto* dt = dynamic_cast<DefTypeStmt*>(prog.lines[0].statements[0].get());
    ASSERT_NE(dt, nullptr);
    EXPECT_EQ(dt->kind, DefTypeStmt::Kind::Int);
    ASSERT_EQ(dt->ranges.size(), 1u);
    EXPECT_EQ(dt->ranges[0].first, 'A');
    EXPECT_EQ(dt->ranges[0].second, 'A');
}

