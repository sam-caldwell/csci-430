// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/PrintStmt.h"

using namespace gwbasic;

/***
 * Test: Parser.Lprint_SyntaxVariants
 * Purpose: Ensure LPRINT and LPRINT USING parse into PrintStmt with
 * implicit printer channel.
 */
TEST(Parser, Lprint_SyntaxVariants) {
    const char* src =
        "10 LPRINT \"X\"\n"
        "20 LPRINT USING(\"%d\"), 3\n";
    Lexer lx(src);
    auto toks = lx.tokenize();
    Parser p(std::move(toks));
    Program prog = p.parseProgram();
    ASSERT_EQ(prog.lines.size(), 2u);
    for (const auto& line : prog.lines) {
        ASSERT_EQ(line.statements.size(), 1u);
        auto* pr = dyn_cast<PrintStmt>(line.statements[0].get());
        ASSERT_NE(pr, nullptr);
        EXPECT_EQ(pr->channel, 1);
    }
}

