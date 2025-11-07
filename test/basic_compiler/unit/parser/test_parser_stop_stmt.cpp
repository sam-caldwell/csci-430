// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/StopStmt.h"

using namespace gwbasic;

/***
 * Test: Parser.StopStmt
 * Purpose: Ensure STOP parses to StopStmt.
 * Components Under Test: Parser::parse STOP.
 * Expected Behavior: First statement is a StopStmt.
 */
TEST(Parser, StopStmt) {
    std::string src = "10 STOP\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto [lines] = p.parseProgram();
    ASSERT_EQ(lines.size(), 1u);
    auto* es = dynamic_cast<StopStmt*>(lines[0].statements[0].get());
    ASSERT_NE(es, nullptr);
}
