// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
Test: SemanticsScreen.Rejects_Wrong_Arity_ZeroOneFour
Inputs: X=SCREEN(), X=SCREEN(1), X=SCREEN(1,1,1,1)
Code under test: Lexer, Parser, SemanticAnalyzer for SCREEN arity
Expected behavior: SemanticError thrown for arg counts other than 2 or 3
*/
TEST(SemanticsScreen, Rejects_Wrong_Arity_ZeroOneFour) {
    auto analyze_src = [](const std::string& src) {
        Lexer lex(src);
        auto toks = lex.tokenize();
        Parser p(std::move(toks));
        auto prog = p.parseProgram();
        SemanticAnalyzer sema;
        return sema.analyze(prog);
    };

    EXPECT_THROW({ (void)analyze_src("10 X=SCREEN()\n"); }, SemanticError);
    EXPECT_THROW({ (void)analyze_src("10 X=SCREEN(1)\n"); }, SemanticError);
    EXPECT_THROW({ (void)analyze_src("10 X=SCREEN(1,1,1,1)\n"); }, SemanticError);
}
