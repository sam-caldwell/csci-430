// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <stdexcept>
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include "logger/Logger.h"
#include "basic_compiler/token/Token.h"
#include "basic_compiler/ast/Program.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/parser/ParseError.h"
#include "basic_compiler/ast/Traits.h"
#include "basic_compiler/ast/DefTypeStmt.h"

namespace gwbasic {

/**
 * Parser: Builds an AST (Program) from a token stream.
 *
 * Purpose:
 *  - Validate syntax and produce a structured representation of input code.
 */
class Parser {
public:
    /**
     * Function: Parser::Parser
     * Purpose:
     *  - Construct a parser instance over a provided token sequence.
     * Inputs:
     *  - tokens: Full token list including NewLine and EndOfFile
     * Outputs:
     *  - Parser object with internal cursor set to beginning
     */
    explicit Parser(std::vector<Token> tokens)
        : tokens_(std::move(tokens)) {}

    /**
     * Function: Parser::parseProgram
     * Purpose:
     *  - Parse all tokens into a Program AST comprised of ordered Lines.
     * Inputs:
     *  - none (consumes internal cursor state)
     * Outputs:
     *  - Program: AST with lines and statements in ascending line order
     * Theory of operation:
     *  - Repeatedly calls parseLine() until EndOfFile is reached.
     */
    Program parseProgram();

private:
    /*
     * Property: tokens_
     * Purpose:
     *  - Complete token stream provided by the lexer.
     * Notes:
     *  - Includes NewLine tokens and a terminating EndOfFile.
     */
    std::vector<Token> tokens_{};

    /*
     * Property: pos_
     * Purpose:
     *  - Current index into 'tokens_' (0-based).
     * Notes:
     *  - Advanced by advance(); bounds-checked by atEnd().
     */
    size_t pos_{0};

    // Syntax-phase logging via ostream-based logger
    logger::Logger syntaxLogger_{};

    /**
     * Function: Parser::peek
     * Purpose:
     *  - Return the current token without consuming it.
     * Inputs:
     *  - none
     * Outputs:
     *  - const Token&: reference to current token
     */
    const Token& peek() const { return tokens_[pos_]; }

    /**
     * Function: Parser::peekNext
     * Purpose:
     *  - Look ahead one token without consuming it.
     * Inputs:
     *  - none
     * Outputs:
     *  - const Token&: reference to next token
     */
    const Token& peekNext() const { return tokens_[pos_ + 1]; }

    /**
     * Function: Parser::advance
     * Purpose:
     *  - Consume and return the current token.
     * Inputs:
     *  - none
     * Outputs:
     *  - const Token&: reference to the consumed token
     */
    const Token& advance() { return tokens_[pos_++]; }

    /**
     * Function: Parser::atEnd
     * Purpose:
     *  - Report if the cursor is at EndOfFile token.
     * Inputs:
     *  - none
     * Outputs:
     *  - bool: true when peek().type == EndOfFile
     */
    bool atEnd() const { return peek().type == TokenType::EndOfFile; }

    /**
        * Function: Parser::check
        * Purpose:
        *  - Test whether the current token matches the given type.
        * Inputs:
        *  - t: Token type to compare against current token
        * Outputs:
        *  - bool: true if current token type equals 't'
        */
    bool check(const TokenType t) const { return peek().type == t; }

    /**
     * Function: Parser::match
     * Purpose:
     *  - If the current token matches 't', consume it and return true.
     * Inputs:
     *  - t: Token type to match
     * Outputs:
     *  - bool: true if matched and consumed; false otherwise
     */
    bool match(const TokenType t) { if (check(t)) { advance(); return true; } return false; }

    /**
     * Function: Parser::consume
     * Purpose:
     *  - Require a specific token type or throw ParseError with context.
     * Inputs:
     *  - t: required token type
     *  - what: human-friendly expectation description for error reporting
     * Outputs:
     *  - void (advances on success)
     */
    void consume(TokenType t, const std::string& what);
    
    /**
     * Function: Parser::parseLine
     * Purpose:
     *  - Parse a line number and its colon-separated statements.
     * Inputs:
     *  - none
     * Outputs:
     *  - Line: AST node representing a single program line
     */
    Line parseLine();
    /**
     * Function: Parser::parseStatement
     * Purpose:
     *  - Parse a single BASIC statement based on the next token.
     * Inputs:
     *  - none
     * Outputs:
     *  - std::unique_ptr<Stmt>: Parsed statement node
     */
    std::unique_ptr<Stmt> parseStatement();
    /**
     * Function: Parser::parsePrint
     * Purpose:
     *  - Parse a PRINT statement with comma/colon-separated items.
     * Outputs:
     *  - Stmt: PrintStmt node
     */
    std::unique_ptr<Stmt> parsePrint();
    /**
     * Function: Parser::parseAssignOrLet
     * Purpose:
     *  - Parse assignment with or without the optional LET keyword.
     * Outputs:
     *  - Stmt: AssignStmt node
     */
    std::unique_ptr<Stmt> parseAssignOrLet();
    /**
     * Function: Parser::parseIf
     * Purpose:
     *  - Parse IF <expr> THEN <line> branching form.
     * Outputs:
     *  - Stmt: IfStmt node
     */
    std::unique_ptr<Stmt> parseIf();
    /**
     * Function: Parser::parseWhile
     * Purpose:
     *  - Parse a WHILE ... WEND construct (inline or multiline).
     * Outputs:
     *  - Stmt: WhileStmt node
     */
    std::unique_ptr<Stmt> parseWhile();
    /**
     * Function: Parser::parseFor
     * Purpose:
     *  - Parse a FOR ... NEXT loop (inline or multiline body).
     * Outputs:
     *  - Stmt: ForStmt node
     */
    std::unique_ptr<Stmt> parseFor();
    /**
     * Function: Parser::parseRun
     * Purpose:
     *  - Parse RUN [<line>] statement.
     * Outputs:
     *  - Stmt: RunStmt node
     */
    std::unique_ptr<Stmt> parseRun();
    /**
     * Function: Parser::parseCommon
     * Purpose:
     *  - Parse COMMON <id>[,<id>...] declaration.
     * Outputs:
     *  - Stmt: CommonStmt node
     */
    std::unique_ptr<Stmt> parseCommon();
    /**
     * Function: Parser::parseChain
     * Purpose:
     *  - Parse CHAIN ["file"][,<line>][,ALL] transfer.
     * Outputs:
     *  - Stmt: ChainStmt node
     */
    std::unique_ptr<Stmt> parseChain();
    /**
     * Function: Parser::parseMerge
     * Purpose:
     *  - Parse MERGE "file" directive (strict filename string required).
     * Outputs:
     *  - Stmt: MergeStmt node
     */
    std::unique_ptr<Stmt> parseMerge();
    /** Parse DIM name '(' length ')' (1-D arrays only). */
    std::unique_ptr<Stmt> parseDim();
    /** Parse OPEN filename FOR (INPUT|OUTPUT) AS #n */
    std::unique_ptr<Stmt> parseOpen();
    /** Parse CLOSE #n */
    std::unique_ptr<Stmt> parseClose();
    /** Parse DATA item[,item...] */
    std::unique_ptr<Stmt> parseData();
    /** Parse READ var[,var...] */
    std::unique_ptr<Stmt> parseRead();
    /** Parse RESTORE */
    std::unique_ptr<Stmt> parseRestore();
    /** Parse WRITE [#n,] expr[,expr...] */
    std::unique_ptr<Stmt> parseWrite();
    /** Parse INPUT with optional #n, varlist or simple INPUT var */
    std::unique_ptr<Stmt> parseInput();
    /** Parse LINE INPUT [#n,] var$ */
    std::unique_ptr<Stmt> parseLineInput();
    /** Parse DEF FNname(param) = expression */
    std::unique_ptr<Stmt> parseDefFn();
    /** Parse DEFSTR/DEFINT/DEFSNG/DEFDBL letter range list */
    std::unique_ptr<Stmt> parseDefType(DefTypeStmt::Kind k);
    /** Parse DEF SEG [= expr] */
    std::unique_ptr<Stmt> parseDefSeg();
    /** Parse BLOAD "file"[,offset] */
    std::unique_ptr<Stmt> parseBload();
    /** Parse BSAVE "file",offset,length */
    std::unique_ptr<Stmt> parseBsave();
    /** Parse POKE address, value */
    std::unique_ptr<Stmt> parsePoke();
    /** Parse CALL address */
    std::unique_ptr<Stmt> parseCallAbs();
    /** Parse DEF USR[digits] = expr */
    std::unique_ptr<Stmt> parseDefUsr();
    /** Parse CHDIR string-expr */
    std::unique_ptr<Stmt> parseChdir();
    /**
     * Function: Parser::parseExpression
     * Purpose:
     *  - Parse the lowest-precedence expression (additive).
     * Outputs:
     *  - Expr: Expression node
     */
    std::unique_ptr<Expr> parseExpression();
    /**
     * Function: Parser::parseComparison
     * Purpose:
     *  - Parse comparison expressions (=, <>, <, <=, >, >=).
     * Outputs:
     *  - Expr: Expression node
     */
    std::unique_ptr<Expr> parseComparison();
    /**
     * Function: Parser::parseTerm
     * Purpose:
     *  - Parse additive expressions (+, -).
     * Outputs:
     *  - Expr: Expression node
     */
    std::unique_ptr<Expr> parseTerm();
    /**
     * Function: Parser::parseFactor
     * Purpose:
     *  - Parse multiplicative expressions (*, /).
     * Outputs:
     *  - Expr: Expression node
     */
    std::unique_ptr<Expr> parseFactor();
    /**
     * Function: Parser::parseUnary
     * Purpose:
     *  - Parse unary prefix operators (+/-) and forward to primary.
     * Outputs:
     *  - Expr: Expression node
     */
    std::unique_ptr<Expr> parseUnary();
    /**
     * Function: Parser::parsePrimary
     * Purpose:
     *  - Parse primary expressions (numbers, identifiers, calls, parens,
     *    string literals).
     * Outputs:
     *  - Expr: Expression node
     */
    std::unique_ptr<Expr> parsePrimary();

public:
    /**
     * Function: Parser::setSyntaxLogPath
     * Purpose:
     *  - Enable syntax analysis logging to the specified file path.
     * Inputs:
     *  - path: Filesystem path to write syntax-phase log entries
     * Outputs:
     *  - void (opens/truncates the file and enables logging)
     */
    void setSyntaxLogPath(const std::string& path);
    // Stream accessor: syntax-phase logger (ostream sink when disabled)
    std::ostream& syntax() { return syntaxLogger_.stream(); }
    /**
     * Function: Parser::setSourcePath
     * Purpose:
     *  - Inform the parser of the source file path for resolving MERGE/RUN/CHAIN.
     * Inputs:
     *  - path: Absolute or base path to the .bas file
     * Outputs:
     *  - void (stores path for later relative-resolution)
     */
    void setSourcePath(const std::string& path) { sourcePath_ = path; }
private:
    // No logSyntax() helper; use syntax() << ... << '\n' directly.

    /**
     * Function: Parser::nodeName
     * Purpose:
     *  - Return a pretty node name for syntax logging.
     * Inputs:
     *  - s: Statement pointer (may be null)
     * Outputs:
     *  - const char*: Human-readable kind name
     */
    static const char* nodeName(const Stmt* s);

    /*
     * Property: sourcePath_
     * Purpose:
     *  - Base filesystem path for resolving relative filenames in MERGE,
     *    RUN, and CHAIN commands.
     * Notes:
     *  - Typically set when compiling from a file, empty when compiling
     *    from a raw string.
     */
    std::string sourcePath_{};
};

} // namespace gwbasic
