// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
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
/*
Test: SemanticsIntrinsics.LogRejectsComparisonArgument
Inputs: Parsed AST (from BASIC snippet) and default environment
Code under test: Semantics analyzer (type/arity/domain checks)
Expected behavior: Valid programs accepted; invalid ones produce expected semantic errors
*/
TEST(SemanticsIntrinsics, LogRejectsComparisonArgument) {
    const std::string src = "10 PRINT LOG(1>0)\n";
    EXPECT_THROW({ (void)analyze_src(src); }, SemanticError);
}

