// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/PrintStmt.h"
#include "basic_compiler/ast/StringExpr.h"

using namespace gwbasic;
/*
 * Test Suite: Parser PRINT (string with doubled quotes)
 * Purpose: Ensure GW-BASIC doubled quote syntax ("") inside strings is
 *          decoded to a single double quote in the literal value.
 */
/***
Test: Parser.PrintStringWithDoubledQuotes
Purpose: Parse a string literal containing GW-BASIC doubled quotes inside PRINT.
Components Under Test: Lexer/Parser string literal decoding.
Expected Behavior: StringExpr value includes a single quote where doubled in source.
*/
TEST(Parser, PrintStringWithDoubledQuotes) {
    // BASIC: 10 PRINT "He said ""OK"""
    const std::string src = "10 PRINT \"He said \"\"OK\"\"\"\n";
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
    EXPECT_EQ(se->value, "He said \"OK\"");
}
