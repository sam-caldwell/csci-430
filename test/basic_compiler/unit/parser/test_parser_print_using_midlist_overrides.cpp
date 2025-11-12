// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/PrintStmt.h"

using namespace gwbasic;

/***
Test: Parser.PrintUsing_MidList_Overrides
Inputs: PRINT USING "%d"; 1, USING "%f"; 2.5, "x"
Expected: Items parsed; last format recorded (string expr); separators and trail correct.
*/
TEST(Parser, PrintUsing_MidList_Overrides) {
    const std::string src = "10 PRINT USING \"%d\"; 1, USING \"%f\"; 2.5, \"x\"\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();
    ASSERT_EQ(prog.lines.size(), 1u);
    auto* ps = dynamic_cast<PrintStmt*>(prog.lines[0].statements[0].get());
    ASSERT_NE(ps, nullptr);
    ASSERT_NE(ps->value, nullptr);
    // Expect 3 items total
    ASSERT_EQ(ps->more.size(), 2u);
    // Last format ("%f") should be set
    ASSERT_NE(ps->format, nullptr);
    // Trailing newline default
    EXPECT_EQ(ps->trail, PrintStmt::Terminator::Newline);
}

