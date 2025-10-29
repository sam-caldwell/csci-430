// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/ForStmt.h"
#include "basic_compiler/ast/PrintStmt.h"
#include "basic_compiler/ast/VarExpr.h"

using namespace gwbasic;
/*
 * Test Suite: Parser FOR (inline body)
 * Purpose: Validate inline body parsing with statements separated by ':' and
 *          terminated by NEXT on the same line.
 * Components Under Test: Parser parseFor/parseStatement sequencing.
 * Expected Behavior: ForStmt contains expected sequence of body statements.
 */
TEST(Parser, ForNextInlineBody) {
    std::string src = "10 FOR I = 1 TO 3 : PRINT I : NEXT I\n";
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto [lines] = p.parseProgram();
    ASSERT_EQ(lines.size(), 1u);
    ASSERT_EQ(lines[0].statements.size(), 1u);
    auto* fs = dynamic_cast<ForStmt*>(lines[0].statements[0].get());
    ASSERT_NE(fs, nullptr);
    EXPECT_EQ(fs->var, "I");
    ASSERT_EQ(fs->body.size(), 1u);
    auto* pr = dynamic_cast<PrintStmt*>(fs->body[0].get());
    ASSERT_NE(pr, nullptr);
    auto* ve = dynamic_cast<VarExpr*>(pr->value.get());
    ASSERT_NE(ve, nullptr);
    EXPECT_EQ(ve->name, "I");
}
