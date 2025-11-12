// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include <sstream>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/PrintStmt.h"

using namespace gwbasic;

/***
Test: Parser.Print_Separators_And_Trailing
Purpose: Parse PRINT with semicolon/comma separators and a trailing semicolon terminator.
Components Under Test: Parser::parsePrint; item/separator/terminator handling.
Expected Behavior: Captures three items, separator kinds, and trailing semicolon.
*/
TEST(Parser, Print_Separators_And_Trailing) {
    const std::string src = "10 PRINT 1;2,3;\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto [lines] = p.parseProgram();
    ASSERT_EQ(lines.size(), 1u);
    ASSERT_EQ(lines[0].statements.size(), 1u);
    auto* ps = dynamic_cast<PrintStmt*>(lines[0].statements[0].get());
    ASSERT_NE(ps, nullptr);
    // Expect 3 items: 1 ; 2 , 3 ;
    ASSERT_NE(ps->value, nullptr);
    ASSERT_EQ(ps->more.size(), 2u);
    ASSERT_GE(ps->seps.size(), 2u);
    EXPECT_EQ(ps->seps[0], PrintStmt::Sep::Semicolon);
    EXPECT_EQ(ps->seps[1], PrintStmt::Sep::Comma);
    EXPECT_EQ(ps->trail, PrintStmt::Terminator::Semicolon);
}
