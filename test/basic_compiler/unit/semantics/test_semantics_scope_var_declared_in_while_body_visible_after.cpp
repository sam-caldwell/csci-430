// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <sstream>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"

using namespace gwbasic;

/***
 * Test: SemanticsScope.VarDeclaredInWhileBodyVisibleAfter
 * Purpose: Validate that a variable declared inside a WHILE loop body remains visible after the loop.
 * Components Under Test: Lexer::tokenize; Parser::parseProgram; SemanticAnalyzer::analyze
 * Expected Behavior: Semantic analysis declares variable W so it is usable on a later line
 *                    (result variables contain "W").
 */
/*
Test: SemanticsScope.VarDeclaredInWhileBodyVisibleAfter
Inputs: Parsed AST (from BASIC snippet) and default environment
Code under test: Semantics analyzer (type/arity/domain checks)
Expected behavior: Valid programs accepted; invalid ones produce expected semantic errors
*/
TEST(SemanticsScope, VarDeclaredInWhileBodyVisibleAfter) {
    const std::string src =
        "10 LET C = 1\n"
        "20 WHILE C < 3\n"
        "30 LET W = C\n"
        "40 LET C = C + 1\n"
        "50 WEND\n"
        "60 PRINT W\n";
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();

    const std::filesystem::path logDir = std::filesystem::path("..") / "basic_compiler";
    std::filesystem::create_directories(logDir);
    const std::filesystem::path logPath = logDir / "sem_scope_while.log";
    std::error_code ec; std::filesystem::remove(logPath, ec);

    SemanticAnalyzer sema;
    sema.setLogPath(logPath.string());
    auto res = sema.analyze(prog);
    EXPECT_TRUE(res.variables.contains("W"));
}
