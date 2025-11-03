// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/RunStmt.h"

using namespace gwbasic;

/***
 * Test: Parser.Run_WithFile_Parses
 * Purpose: Validate parsing of RUN with a filename only (no target line).
 * Components Under Test: Lexer::tokenize; Parser::parseProgram; AST RunStmt
 * Expected Behavior: Produces a single RunStmt with targetLine unset.
 */
TEST(Parser, Run_WithFile_Parses) {
    std::string src = "10 RUN \"demo.bas\"\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto [lines] = p.parseProgram();
    ASSERT_EQ(lines.size(), 1u);
    auto* rs = dynamic_cast<RunStmt*>(lines[0].statements[0].get());
    ASSERT_NE(rs, nullptr);
    EXPECT_FALSE(rs->targetLine.has_value());
}
