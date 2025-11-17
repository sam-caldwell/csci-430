// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"

using namespace gwbasic;

/***
 * Test: SemanticsScope.VarDeclaredInIfBodyVisibleAfter
 * Purpose: Validate that a variable declared inside an IF body remains visible after the block.
 * Components Under Test: Lexer::tokenize; Parser::parseProgram; SemanticAnalyzer::analyze
 * Expected Behavior: Semantic analysis declares variable A so it is usable on a later line
 *                    (result variables contain "A").
 */
/*
Test: SemanticsScope.VarDeclaredInIfBodyVisibleAfter
Inputs: Parsed AST (from BASIC snippet) and default environment
Code under test: Semantics analyzer (type/arity/domain checks)
Expected behavior: Valid programs accepted; invalid ones produce expected semantic errors
*/

TEST(SemanticsScope, VarDeclaredInIfBodyVisibleAfter) {
    const std::string src =
        "10 IF 1 < 2 THEN\n"
        "20 LET A = 1\n"
        "30 END IF\n"
        "40 PRINT A\n";
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();

    SemanticAnalyzer sema;
    auto res = sema.analyze(prog);
    // Variable first seen in IF body should be globally declared and usable later
    EXPECT_TRUE(res.variables.contains("A"));
}
