// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/PrintStmt.h"
#include "basic_compiler/ast/StringExpr.h"

using namespace gwbasic;
/*
 * Test Suite: Parser PRINT (string)
 * Purpose: Verify string literal parsing in PRINT statements.
 * Components Under Test: Parser parsePrint; string literal handling.
 * Expected Behavior: PrintStmt contains StringExpr with expected contents.
 */
TEST(Parser, PrintString) {
    std::string src = "10 PRINT \"Hello\"\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto [lines] = p.parseProgram();
    ASSERT_EQ(lines.size(), 1u);
    ASSERT_EQ(lines[0].statements.size(), 1u);
    auto* ps = dynamic_cast<PrintStmt*>(lines[0].statements[0].get());
    ASSERT_NE(ps, nullptr);
    auto* se = dynamic_cast<StringExpr*>(ps->value.get());
    ASSERT_NE(se, nullptr);
    EXPECT_EQ(se->value, "Hello");
}
