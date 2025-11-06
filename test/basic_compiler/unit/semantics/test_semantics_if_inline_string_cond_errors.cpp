// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"

using namespace gwbasic;

/***
 * Test: Semantics.IfInline_StringConditionErrors
 * Purpose: Ensure IF inline blocks reject string conditions during semantics.
 */
/*
Test: Semantics.IfInline_StringConditionErrors
Inputs: Program with IF "A" THEN A=1 (string condition)
Code under test: SemanticAnalyzer::analyze
Expected behavior: Throws SemanticError
*/
TEST(Semantics, IfInline_StringConditionErrors) {
    const char* src =
        "10 IF \"A\" THEN A=1\n"
        "20 END\n";
    Lexer lex(src);
    auto tokens = lex.tokenize();
    Parser p(std::move(tokens));
    auto prog = p.parseProgram();
    SemanticAnalyzer sema;
    EXPECT_THROW({ auto res = sema.analyze(prog); (void)res; }, SemanticError);
}

