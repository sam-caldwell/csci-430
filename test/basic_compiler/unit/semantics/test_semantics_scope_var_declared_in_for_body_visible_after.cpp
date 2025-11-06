// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <sstream>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"

using namespace gwbasic;

/***
 * Test: SemanticsScope.VarDeclaredInForBodyVisibleAfter
 * Purpose: Validate that a variable declared inside a FOR loop body remains visible after the loop.
 * Components Under Test: Lexer::tokenize; Parser::parseProgram; SemanticAnalyzer::analyze
 * Expected Behavior: Semantic analysis declares variable Z so it is usable on a later line
 *                    (result variables contain "Z").
 */
/*
Test: SemanticsScope.VarDeclaredInForBodyVisibleAfter
Inputs: Parsed AST (from BASIC snippet) and default environment
Code under test: Semantics analyzer (type/arity/domain checks)
Expected behavior: Valid programs accepted; invalid ones produce expected semantic errors
*/
TEST(SemanticsScope, VarDeclaredInForBodyVisibleAfter) {
    const std::string src =
        "10 FOR I = 1 TO 2\n"
        "20 LET Z = I\n"
        "30 NEXT I\n"
        "40 PRINT Z\n";
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();

    std::filesystem::path logDir = std::filesystem::path("..") / "basic_compiler";
    std::filesystem::create_directories(logDir);
    std::filesystem::path logPath = logDir / "sem_scope_for.log";
    std::error_code ec; std::filesystem::remove(logPath, ec);

    SemanticAnalyzer sema;
    sema.setLogPath(logPath.string());
    auto res = sema.analyze(prog);
    EXPECT_TRUE(res.variables.contains("Z"));
}
