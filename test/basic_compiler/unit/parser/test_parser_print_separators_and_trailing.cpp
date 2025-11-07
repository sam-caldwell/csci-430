// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include <sstream>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/PrintStmt.h"

using namespace gwbasic;

/*
 * Test: Parser.Print_Separators_And_Trailing
 * Purpose: Ensure PRINT parses semicolon/comma separators and trailing
 *          semicolon without error, and stores them in PrintStmt.
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

/*
 * Test: Parser.Print_Using_Semicolon_List
 * Purpose: Leading PRINT USING with required separator accepted and list
 *          parsed; last trailing comma recorded when present.
 */
TEST(Parser, Print_Using_Semicolon_List) {
    const std::string src = "10 PRINT USING(\"%d\"); 7, 8, 9,\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto [lines] = p.parseProgram();
    ASSERT_EQ(lines.size(), 1u);
    ASSERT_EQ(lines[0].statements.size(), 1u);
    auto* ps = dynamic_cast<PrintStmt*>(lines[0].statements[0].get());
    ASSERT_NE(ps, nullptr);
    // 3 items, 3-1=2 separators
    ASSERT_NE(ps->value, nullptr);
    ASSERT_EQ(ps->more.size(), 2u);
    ASSERT_EQ(ps->seps.size(), 2u);
    EXPECT_EQ(ps->trail, PrintStmt::Terminator::Comma);
}
