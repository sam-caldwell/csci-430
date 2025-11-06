// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/parser/ParseError.h"

using namespace gwbasic;

/*
Test: Parser.ParseProgramUnmatchedMarkers
Inputs: Lines with structural markers not inside matching blocks
Code under test: Parser::parseProgram block restructuring and error checks
Expected behavior: Throws ParseError with appropriate diagnostics
*/
TEST(Parser, ParseProgram_UnmatchedMarkers_ELSE) {
    std::string src = "10 ELSE\n";
    Lexer lx(src); auto toks = lx.tokenize();
    Parser p(std::move(toks));
    EXPECT_THROW({ auto _ = p.parseProgram(); }, ParseError);
}

TEST(Parser, ParseProgram_UnmatchedMarkers_ENDIF) {
    std::string src = "10 END IF\n";
    Lexer lx(src); auto toks = lx.tokenize();
    Parser p(std::move(toks));
    EXPECT_THROW({ auto _ = p.parseProgram(); }, ParseError);
}

TEST(Parser, ParseProgram_UnmatchedMarkers_NEXT) {
    std::string src = "10 NEXT\n";
    Lexer lx(src); auto toks = lx.tokenize();
    Parser p(std::move(toks));
    EXPECT_THROW({ auto _ = p.parseProgram(); }, ParseError);
}

TEST(Parser, ParseProgram_UnmatchedMarkers_WEND) {
    std::string src = "10 WEND\n";
    Lexer lx(src); auto toks = lx.tokenize();
    Parser p(std::move(toks));
    EXPECT_THROW({ auto _ = p.parseProgram(); }, ParseError);
}

