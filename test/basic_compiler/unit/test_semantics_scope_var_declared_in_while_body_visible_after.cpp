// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <sstream>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"

using namespace gwbasic;

/***
 * Test: SemanticsScope.VarDeclaredInWhileBodyVisibleAfter
 * Purpose: Validate that a variable declared inside a WHILE loop body remains visible after the loop.
 * Components Under Test: Lexer::tokenize; Parser::parseProgram; SemanticAnalyzer::analyze
 * Expected Behavior: Semantic analysis declares variable W so it is usable on a later line
 *                    (result variables contain "W").
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

    std::filesystem::path logDir = std::filesystem::path("..") / "basic_compiler";
    std::filesystem::create_directories(logDir);
    std::filesystem::path logPath = logDir / "sem_scope_while.log";
    std::error_code ec; std::filesystem::remove(logPath, ec);

    SemanticAnalyzer sema;
    sema.setLogPath(logPath.string());
    auto res = sema.analyze(prog);
    EXPECT_TRUE(res.variables.contains("W"));
}
