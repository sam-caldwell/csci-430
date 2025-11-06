// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"

using namespace gwbasic;

static SemanticAnalyzer::Result analyze_src(const std::string& src) {
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();
    SemanticAnalyzer sema;
    return sema.analyze(prog);
}

/*
 * Test: SemanticsScreen.Accepts_Two_Args
 * Inputs: SCREEN(1,1)
 * Code under test: Lexer, Parser, SemanticAnalyzer for SCREEN arity/type.
 * Expected behavior: No semantic error for 2 numeric args.
 */
TEST(SemanticsScreen, Accepts_Two_Args) {
    const std::string src = "10 X=SCREEN(1,1)\n";
    EXPECT_NO_THROW({ (void)analyze_src(src); });
}
