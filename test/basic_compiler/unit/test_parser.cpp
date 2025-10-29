
#include <gtest/gtest.h>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"

using namespace gwbasic;
/*
 * Test Suite: Parser Smoke
 * Purpose: Basic integration of Lexer + Parser on a tiny program,
 *          verifying minimal structure builds without errors.
 * Components Under Test: Lexer; Parser; Program/Line composition.
 * Expected Behavior: Parsed Program contains expected number of lines/statements.
 */
TEST(Parser, ParseSimpleProgram) {
    std::string src = R"(10 LET A = 7
20 PRINT A
30 IF A > 3 THEN 50
40 PRINT "Done"
50 END
)";
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto [lines] = p.parseProgram();
    ASSERT_EQ(lines.size(), 5u);
    EXPECT_EQ(lines[0].number, 10);
    EXPECT_FALSE(lines[0].statements.empty());
}
