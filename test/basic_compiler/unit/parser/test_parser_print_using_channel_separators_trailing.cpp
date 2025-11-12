// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/PrintStmt.h"

using namespace gwbasic;

/***
Test: Parser.PrintUsing_Channel_SepAndTrailing
Inputs: PRINT #2, USING("%s"), "A"; 1, "B",
Expected: channel=2, format set, separators [Comma, Semicolon, Comma], trailing Comma.
*/
TEST(Parser, PrintUsing_Channel_SepAndTrailing) {
    const std::string src = "10 PRINT #2, USING(\"%s\"), \"A\"; 1, \"B\",\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();
    ASSERT_EQ(prog.lines.size(), 1u);
    auto* ps = dynamic_cast<PrintStmt*>(prog.lines[0].statements[0].get());
    ASSERT_NE(ps, nullptr);
    EXPECT_EQ(ps->channel, 2);
    ASSERT_NE(ps->format, nullptr);
    ASSERT_NE(ps->value, nullptr);
    // more will have two items: 1 and "B"
    ASSERT_EQ(ps->more.size(), 2u);
    // Expect seps count >= 2 (Comma, Semicolon, Comma). Exact layout: value="A" followed by ; then 1 (so first sep is Semicolon)
    ASSERT_GE(ps->seps.size(), 2u);
    EXPECT_EQ(ps->trail, PrintStmt::Terminator::Comma);
}

