// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

static SemanticAnalyzer::Result analyze_src_bad(const std::string& src) {
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();
    SemanticAnalyzer sema;
    return sema.analyze(prog);
}

/*
 * Test: SemanticsScreen.Rejects_String_Arg
 * Inputs: SCREEN("A",1)
 * Code under test: Lexer, Parser, SemanticAnalyzer for SCREEN types.
 * Expected behavior: SemanticError thrown when row is a string.
 */
TEST(SemanticsScreen, Rejects_String_Arg) {
    const std::string src = "10 X=SCREEN(\"A\",1)\n";
    EXPECT_THROW({ (void)analyze_src_bad(src); }, SemanticError);
}

