// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"

using namespace gwbasic;
/*
 * Test Suite: Parser Error (GOTO missing line)
 * Purpose: Ensure GOTO requires a numeric line target.
 * Components Under Test: Parser parseStatement for GOTO.
 * Expected Behavior: Error reported when line number is absent.
 */
TEST(Parser, ErrorGotoMissingLine) {
    std::string src = "10 GOTO\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    EXPECT_THROW({ auto prog = p.parseProgram(); (void)prog; }, ParseError);
}
