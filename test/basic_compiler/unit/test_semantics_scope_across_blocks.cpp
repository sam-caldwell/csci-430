// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"

using namespace gwbasic;


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

    std::filesystem::path logDir = std::filesystem::path("..") / "basic_compiler";
    std::filesystem::create_directories(logDir);
    std::filesystem::path logPath = logDir / "sem_scope_if.log";
    std::error_code ec; std::filesystem::remove(logPath, ec);

    SemanticAnalyzer sema;
    sema.setLogPath(logPath.string());
    auto res = sema.analyze(prog);
    // Variable first seen in IF body should be globally declared and usable later
    EXPECT_TRUE(res.variables.contains("A"));
}

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
