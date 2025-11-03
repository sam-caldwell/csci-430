// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <set>
#include <sstream>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"

using namespace gwbasic;

/*
 * Test Suite: Semantics COMMON
 * Purpose: Ensure COMMON declares listed variables globally.
 */
TEST(SemanticsCommon, DeclaresVariables) {
    std::string src = "10 COMMON A, B\n20 PRINT A\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto program = p.parseProgram();

    SemanticAnalyzer sema;
    auto res = sema.analyze(program);
    // Expect A and B in the variables set
    EXPECT_TRUE(res.variables.contains("A"));
    EXPECT_TRUE(res.variables.contains("B"));
}

