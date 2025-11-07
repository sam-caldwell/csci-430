// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/OptionBaseStmt.h"

using namespace gwbasic;

/*
 * Test Suite: Parser OPTION BASE
 * Purpose: Ensure the parser recognizes and constructs OptionBaseStmt.
 */
/***
Test: Parser.OptionBaseParses
Purpose: Ensure OPTION BASE parses to OptionBaseStmt with configured base.
Components Under Test: Parser::parseOptionBase.
Expected Behavior: AST contains OptionBaseStmt with base set to 1.
*/
TEST(Parser, OptionBaseParses) {
    std::string src = "10 OPTION BASE 1\n20 END\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto [lines] = p.parseProgram();
    ASSERT_EQ(lines.size(), 2u);
    ASSERT_EQ(lines[0].statements.size(), 1u);
    auto* ob = dynamic_cast<OptionBaseStmt*>(lines[0].statements[0].get());
    ASSERT_NE(ob, nullptr);
    EXPECT_EQ(ob->base, 1);
}
