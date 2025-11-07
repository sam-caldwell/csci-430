// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/OptionPrintZonesStmt.h"

using namespace gwbasic;

/***
Test: Parser.OptionPrintZonesParses
Inputs: OPTION PRINTZONES ON and OFF
Code under test: Parser::parseProgram
Expected behavior: Produces OptionPrintZonesStmt with enabled set appropriately
*/
TEST(Parser, OptionPrintZonesParses) {
    std::string src = "10 OPTION PRINTZONES ON\n20 OPTION PRINTZONES OFF\n";
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();
    ASSERT_EQ(prog.lines.size(), 2u);
    auto* a = dynamic_cast<OptionPrintZonesStmt*>(prog.lines[0].statements[0].get());
    auto* b = dynamic_cast<OptionPrintZonesStmt*>(prog.lines[1].statements[0].get());
    ASSERT_NE(a, nullptr);
    ASSERT_NE(b, nullptr);
    EXPECT_TRUE(a->enabled);
    EXPECT_FALSE(b->enabled);
}
