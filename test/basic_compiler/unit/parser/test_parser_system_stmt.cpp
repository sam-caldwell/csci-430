// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/SystemStmt.h"

using namespace gwbasic;

/***
 * Test: Parser.SystemStmt
 * Purpose: Ensure SYSTEM parses to SystemStmt.
 * Components Under Test: Parser::parse SYSTEM.
 * Expected Behavior: First statement is a SystemStmt.
 */
TEST(Parser, SystemStmt) {
    std::string src = "10 SYSTEM\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto [lines] = p.parseProgram();
    ASSERT_EQ(lines.size(), 1u);
    auto* es = dynamic_cast<SystemStmt*>(lines[0].statements[0].get());
    ASSERT_NE(es, nullptr);
}
