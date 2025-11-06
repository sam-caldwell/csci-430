// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"

using namespace gwbasic;

/*
 * Test Suite: Semantics OPTION BASE
 * Purpose: Verify OPTION BASE is recorded in semantic result.
 */
/*
Test: SemanticsOptionBase.RecordsBaseInResult
Inputs: Parsed AST (from BASIC snippet) and default environment
Code under test: Semantics analyzer (type/arity/domain checks)
Expected behavior: Valid programs accepted; invalid ones produce expected semantic errors
*/
TEST(SemanticsOptionBase, RecordsBaseInResult) {
    std::string src = "10 OPTION BASE 1\n20 DIM A(3)\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();

    SemanticAnalyzer sema;
    auto res = sema.analyze(prog);
    EXPECT_EQ(res.optionBase, 1);
    ASSERT_TRUE(res.arrays.contains("A"));
    ASSERT_EQ(res.arrays["A"].size(), 1u);
    EXPECT_EQ(res.arrays["A"][0], 3);
}

