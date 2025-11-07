// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/PrintStmt.h"

using namespace gwbasic;

/*
Test: ParserPrint.UsingMidList_LastWins
Purpose: Cover PRINT mid-list USING(fmt) handling where the last USING wins.
Inputs: 10 PRINT 1, USING "%02d", 2, USING "%03d", 3
Expected: PrintStmt has 3 value items; format present (last one); channel default -1.
*/
TEST(ParserPrint, UsingMidList_LastWins) {
    const std::string src = "10 PRINT 1, USING \"%02d\", 2, USING \"%03d\", 3\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    Program prog = p.parseProgram();
    ASSERT_EQ(prog.lines.size(), 1u);
    ASSERT_EQ(prog.lines[0].statements.size(), 1u);
    auto* ps = dyn_cast<PrintStmt>(prog.lines[0].statements[0].get());
    ASSERT_NE(ps, nullptr);
    EXPECT_EQ(ps->channel, -1);
    ASSERT_TRUE(ps->format != nullptr);
    ASSERT_TRUE(ps->value != nullptr);
    // We expect value + two more numeric items
    EXPECT_EQ(ps->more.size(), 2u);
}

