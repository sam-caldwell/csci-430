// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"

using namespace gwbasic;
/*
 * Test Suite: Parser Error (IF missing THEN)
 * Purpose: Ensure IF conditions must be followed by THEN.
 * Components Under Test: Parser parseIf.
 * Expected Behavior: Parser reports error when THEN keyword is missing.
 */
TEST(Parser, ErrorIfMissingThen) {
    std::string src = "10 IF A < 10 50\n"; // missing THEN keyword
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    EXPECT_THROW({ auto prog = p.parseProgram(); (void)prog; }, ParseError);
}
