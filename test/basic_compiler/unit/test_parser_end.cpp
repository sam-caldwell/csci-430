// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/EndStmt.h"

using namespace gwbasic;
/*
 * Test Suite: Parser END
 * Purpose: Verify END statement parses correctly.
 * Components Under Test: Lexer; Parser parseStatement.
 * Expected Behavior: Single EndStmt present for the line.
 */
TEST(Parser, EndStmt) {
    std::string src = "10 END\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto [lines] = p.parseProgram();
    ASSERT_EQ(lines.size(), 1u);
    auto* es = dynamic_cast<EndStmt*>(lines[0].statements[0].get());
    ASSERT_NE(es, nullptr);
}
