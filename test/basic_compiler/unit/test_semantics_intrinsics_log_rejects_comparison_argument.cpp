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
 * Test: SemanticsIntrinsics.LogRejectsComparisonArgument
 * Purpose: Ensure LOG rejects boolean/comparison argument expressions.
 */
TEST(SemanticsIntrinsics, LogRejectsComparisonArgument) {
    const std::string src = "10 PRINT LOG(1>0)\n";
    EXPECT_THROW({ (void)analyze_src(src); }, SemanticError);
}

