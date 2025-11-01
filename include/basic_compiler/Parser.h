// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <stdexcept>
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include "basic_compiler/token/Token.h"
#include "basic_compiler/ast/Program.h"
#include "basic_compiler/ast/RTTI.h"

namespace gwbasic {

/**
 * ParseError: Exception for syntactic errors during parsing.
 *
 * Purpose:
 *  - Surface unexpected tokens or malformed grammar constructs.
 */
class ParseError final : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

/**
 * Parser: Builds an AST (Program) from a token stream.
 *
 * Purpose:
 *  - Validate syntax and produce a structured representation of input code.
 */
class Parser {
public:
    /**
     * Construct the parser.
     *
     * Inputs:
     *  - tokens: Full token list including NewLine and EndOfFile.
     */
    explicit Parser(std::vector<Token> tokens)
        : tokens_(std::move(tokens)) {}

    /**
     * parseProgram: Parse all input tokens into a Program AST.
     *
     * Inputs:
     *  - none (consumes internal state).
     *
     * Outputs:
     *  - Program: AST with ordered line blocks.
     */
    Program parseProgram();

private:
    std::vector<Token> tokens_{};
    size_t pos_{0};
    // Syntax logging
    bool syntaxLogEnabled_{false};
    std::ofstream syntaxLog_;

    const Token& peek() const { return tokens_[pos_]; }
    const Token& peekNext() const { return tokens_[pos_ + 1]; }
    const Token& advance() { return tokens_[pos_++]; }
    bool atEnd() const { return peek().type == TokenType::EndOfFile; }
    bool check(const TokenType t) const { return peek().type == t; }
    bool match(const TokenType t) { if (check(t)) { advance(); return true; } return false; }

    /** consume: Require token of type t or throw ParseError. */
    void consume(TokenType t, const std::string& what);
    
    /** parseLine: Parse a numbered line and its statements. */
    Line parseLine();
    /** parseStatement: Parse a single statement. */
    std::unique_ptr<Stmt> parseStatement();
    /** parsePrint: Parse PRINT. */
    std::unique_ptr<Stmt> parsePrint();
    /** parseAssignOrLet: Parse assignment with or without LET. */
    std::unique_ptr<Stmt> parseAssignOrLet();
    /** parseIf: Parse IF ... THEN <line>. */
    std::unique_ptr<Stmt> parseIf();
    /** parseFor: Parse single-line FOR ... NEXT. */
    std::unique_ptr<Stmt> parseFor();
    /** Expression grammar helpers. */
    std::unique_ptr<Expr> parseExpression();
    std::unique_ptr<Expr> parseComparison();
    std::unique_ptr<Expr> parseTerm();
    std::unique_ptr<Expr> parseFactor();
    std::unique_ptr<Expr> parseUnary();
    std::unique_ptr<Expr> parsePrimary();

public:
    /** Enable syntax analysis logging to the specified file path. */
    void setSyntaxLogPath(const std::string& path) {
        if (syntaxLog_.is_open()) syntaxLog_.close();
        syntaxLog_.open(path, std::ios::out | std::ios::trunc);
        syntaxLogEnabled_ = syntaxLog_.is_open();
    }
private:
    void logSyntax(const std::string& msg) { if (syntaxLogEnabled_ && syntaxLog_.is_open()) syntaxLog_ << msg << '\n'; }
    static const char* nodeName(const Stmt* s) {
        if (isa<AssignStmt>(s)) return "AssignStmt";
        if (isa<PrintStmt>(s)) return "PrintStmt";
        if (isa<GotoStmt>(s)) return "GotoStmt";
        if (isa<GosubStmt>(s)) return "GosubStmt";
        if (isa<ReturnStmt>(s)) return "ReturnStmt";
        if (isa<IfStmt>(s)) return "IfStmt";
        if (isa<InputStmt>(s)) return "InputStmt";
        if (isa<ForStmt>(s)) return "ForStmt";
        if (isa<EndStmt>(s)) return "EndStmt";
        return "Stmt";
    }
};

} // namespace gwbasic
