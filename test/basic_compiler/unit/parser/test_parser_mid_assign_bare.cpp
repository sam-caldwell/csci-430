// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/MidAssignStmt.h"

using namespace gwbasic;

/***
 * Test: Parser.MidAssign_Bare
 * Purpose: Ensure MID$(A$, start) = "..." parses without a length argument.
 * Components Under Test: Parser::parse MID$ assignment.
 * Expected Behavior: MidAssignStmt has name A$ and len is null.
 */
TEST(Parser, MidAssign_Bare) {
    std::string src = "10 MID$(A$,2)=\"XY\"\n";
    Lexer lx(src); auto toks = lx.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();
    ASSERT_EQ(prog.lines.size(), 1u);
    ASSERT_EQ(prog.lines[0].statements.size(), 1u);
    auto* mid = dyn_cast<MidAssignStmt>(prog.lines[0].statements[0].get());
    ASSERT_NE(mid, nullptr);
    EXPECT_EQ(mid->name, std::string("A$"));
    EXPECT_TRUE(mid->len == nullptr);
}
