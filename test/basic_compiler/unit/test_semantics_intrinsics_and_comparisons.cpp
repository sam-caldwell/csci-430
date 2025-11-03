// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"

using namespace gwbasic;

// Helper to run semantics on a source snippet
static SemanticAnalyzer::Result analyze_src(const std::string& src) {
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();
    SemanticAnalyzer sema;
    return sema.analyze(prog);
}

/***
 * Test: SemanticsIntrinsics.KnownFunction_AllowsNumericArg
 * Purpose: Verify known intrinsic SQR accepts numeric argument and passes semantics.
 */
TEST(SemanticsIntrinsics, KnownFunction_AllowsNumericArg) {
    const std::string src = "10 PRINT SQR(9)\n";
    EXPECT_NO_THROW({ (void)analyze_src(src); });
}
