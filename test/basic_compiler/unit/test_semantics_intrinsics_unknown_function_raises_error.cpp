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

/***
 * Test: SemanticsIntrinsics.UnknownFunction_RaisesError
 * Purpose: Ensure unknown function name causes a semantic error.
 */
TEST(SemanticsIntrinsics, UnknownFunction_RaisesError) {
    const std::string src = "10 PRINT FOO(1)\n";
    EXPECT_THROW({ (void)analyze_src(src); }, SemanticError);
}

