// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/MidAssignStmt.h"

using namespace gwbasic;

/***
 * Test: Parser.MidAssign_WithLen_Let
 * Purpose: Ensure LET MID$(S$, start, len) = "..." parses capturing the len expression.
 * Components Under Test: Parser::parse MID$ assignment with len.
 * Expected Behavior: MidAssignStmt has name S$ and non-null len.
 */
TEST(Parser, MidAssign_WithLen_Let) {
    std::string src = "10 LET MID$(S$,3,5)=\"HELLO\"\n";
    Lexer lx(src); auto toks = lx.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();
    ASSERT_EQ(prog.lines.size(), 1u);
    ASSERT_EQ(prog.lines[0].statements.size(), 1u);
    auto* mid = dyn_cast<MidAssignStmt>(prog.lines[0].statements[0].get());
    ASSERT_NE(mid, nullptr);
    EXPECT_EQ(mid->name, std::string("S$"));
    EXPECT_NE(mid->len, nullptr);
}
