// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/RunStmt.h"

using namespace gwbasic;

/*
 * Test Suite: Parser RUN
 * Purpose: Verify strict RUN grammar requires filename and optional line.
 * Components Under Test: Lexer; Parser parseStatement; parseRun.
 * Expected Behavior: Missing filename errors; with filename (and optional
 *                    line) parses and records targetLine.
 */
/*
Test: Parser.Run_MissingFilename_Errors
Inputs: BASIC source snippet
Code under test: Parser (BASIC → AST)
Expected behavior: AST structure or parse errors match expectations
*/
TEST(Parser, Run_MissingFilename_Errors) {
    std::string src = "10 RUN\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    EXPECT_THROW({ (void)p.parseProgram(); }, ParseError);
}
