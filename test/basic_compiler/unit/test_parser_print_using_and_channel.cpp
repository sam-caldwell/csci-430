// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/PrintStmt.h"

using namespace gwbasic;

TEST(ParserPrint, ChannelAndUsingAndItems) {
    const std::string src =
        "10 PRINT #3, USING \"%d %f\", 7, 2.5\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    Program prog = p.parseProgram();
    ASSERT_EQ(prog.lines.size(), 1u);
    ASSERT_EQ(prog.lines[0].statements.size(), 1u);
    auto* ps = dyn_cast<PrintStmt>(prog.lines[0].statements[0].get());
    ASSERT_NE(ps, nullptr);
    EXPECT_EQ(ps->channel, 3);
    ASSERT_TRUE(ps->format != nullptr);
    // Expect two items (value + one more)
    ASSERT_TRUE(ps->value != nullptr);
    EXPECT_EQ(ps->more.size(), 1u);
}

