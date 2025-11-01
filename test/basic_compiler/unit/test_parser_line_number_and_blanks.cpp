// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"

using namespace gwbasic;

/*
 * Test Suite: Parser Line Number and Blank Handling
 */
TEST(Parser, ErrorMissingLeadingLineNumber) {
    const std::string src = "PRINT 1\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    EXPECT_THROW({ auto prog = p.parseProgram(); (void)prog; }, ParseError);
}

TEST(Parser, SkipsLeadingBlankLines) {
    const std::string src = "\n\n10 END\n\n";
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();
    ASSERT_EQ(prog.lines.size(), 1u);
    EXPECT_EQ(prog.lines.front().number, 10);
}
