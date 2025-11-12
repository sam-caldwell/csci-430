// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"

using namespace gwbasic;

/***
 * Test: SemanticsScope.VarDeclaredInIfWithinForVisibleAfter
 * Purpose: Validate that a variable declared inside an IF nested within a FOR loop remains visible after the loop.
 * Components Under Test: Lexer::tokenize; Parser::parseProgram; SemanticAnalyzer::analyze
 * Expected Behavior: Semantic analysis declares variable N so it is usable on a later line
 *                    (result variables contain "N").
 */
/*
Test: SemanticsScope.VarDeclaredInIfWithinForVisibleAfter
Inputs: Parsed AST (from BASIC snippet) and default environment
Code under test: Semantics analyzer (type/arity/domain checks)
Expected behavior: Valid programs accepted; invalid ones produce expected semantic errors
*/
TEST(SemanticsScope, VarDeclaredInIfWithinForVisibleAfter) {
    const std::string src =
        "10 FOR I = 1 TO 1\n"
        "20 IF I = 1 THEN\n"
        "30 LET N = 5\n"
        "40 END IF\n"
        "50 NEXT I\n"
        "60 PRINT N\n";
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();

    SemanticAnalyzer sema;
    auto res = sema.analyze(prog);
    EXPECT_TRUE(res.variables.contains("N"));
}
