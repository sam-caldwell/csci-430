// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"

using namespace gwbasic;

/***
 * Test: SemanticsScope.VarDeclaredInWhileWithinIfVisibleAfter
 * Purpose: Validate that a variable declared inside a WHILE nested within an IF block remains visible after.
 * Components Under Test: Lexer::tokenize; Parser::parseProgram; SemanticAnalyzer::analyze
 * Expected Behavior: Semantic analysis declares variable W so it is usable on a later line
 *                    (result variables contain "W").
 */
/*
Test: SemanticsScope.VarDeclaredInWhileWithinIfVisibleAfter
Inputs: Parsed AST (from BASIC snippet) and default environment
Code under test: Semantics analyzer (type/arity/domain checks)
Expected behavior: Valid programs accepted; invalid ones produce expected semantic errors
*/
TEST(SemanticsScope, VarDeclaredInWhileWithinIfVisibleAfter) {
    const std::string src =
        "10 IF 1 < 2 THEN\n"
        "20 LET C = 1\n"
        "30 WHILE C < 2\n"
        "40 LET W = C\n"
        "50 LET C = C + 1\n"
        "60 WEND\n"
        "70 END IF\n"
        "80 PRINT W\n";
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();

    SemanticAnalyzer sema;
    auto res = sema.analyze(prog);
    EXPECT_TRUE(res.variables.contains("W"));
}
