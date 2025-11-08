// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Parser.h"
#include "basic_compiler/Lexer.h"
#include "basic_compiler/ast/WidthStmt.h"

using namespace gwbasic;

/***
 * Test: Parser.WIDTH_Parses_BothForms
 * Purpose: Ensure WIDTH n and WIDTH dev$, n parse into WidthStmt with fields set.
 */
TEST(Parser, WIDTH_Parses_BothForms) {
    const char* src =
        "10 WIDTH 80\n"
        "20 WIDTH \"LPT1:\", 132\n";
    Lexer lx(src);
    auto tokens = lx.tokenize();
    Parser p(std::move(tokens));
    auto prog = p.parseProgram();
    ASSERT_EQ(prog.lines.size(), 2u);
    {
        const auto* w = dyn_cast<WidthStmt>(prog.lines[0].statements[0].get());
        ASSERT_NE(w, nullptr);
        EXPECT_EQ(w->device, nullptr);
        ASSERT_NE(w->columns, nullptr);
    }
    {
        const auto* w = dyn_cast<WidthStmt>(prog.lines[1].statements[0].get());
        ASSERT_NE(w, nullptr);
        ASSERT_NE(w->device, nullptr);
        ASSERT_NE(w->columns, nullptr);
    }
}

