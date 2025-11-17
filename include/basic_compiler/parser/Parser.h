// NOLINTBEGIN(llvm-header-guard)
#ifndef BASIC_COMPILER_PARSER_H
#define BASIC_COMPILER_PARSER_H
// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <cstddef>
#include <memory>
#include <ostream>
#include <streambuf>
#include <string>
#include <utility>
#include <vector>

#include "basic_compiler/ast/DefTypeStmt.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/Line.h"
#include "basic_compiler/ast/Program.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/token/Token.h"
#include "basic_compiler/token/TokenType.h"
#include "basic_compiler/parser/ParseError.h"

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
    // Helper to fold multi-line blocks (FOR/IF/WHILE) after parsing.
    Program foldProgramBlocks(Program&& prog);
    /*
     * Property: tokens_
     * Purpose:
     *  - Complete token stream provided by the lexer.
     * Notes:
     *  - Includes NewLine tokens and a terminating EndOfFile.
     */
    std::vector<Token> tokens_;

    /*
     * Property: pos_
     * Purpose:
     *  - Current index into 'tokens_' (0-based).
     * Notes:
     *  - Advanced by advance(); bounds-checked by atEnd().
     */
    size_t pos_{0};

    // Syntax-phase logging removed

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
    bool check(const TokenType type) const { return peek().type == type; }

    /**
     * Function: Parser::match
     * Purpose:
     *  - If the current token matches 't', consume it and return true.
     * Inputs:
     *  - t: Token type to match
     * Outputs:
     *  - bool: true if matched and consumed; false otherwise
     */
    bool match(const TokenType type) {
        if (check(type)) {
            advance();
            return true;
        }
        return false;
    }

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
    void consume(TokenType type, const std::string& what);
    
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

    // Helpers used by parseStatement() to reduce complexity. These are
    // intentionally declared private; unit tests may make them visible
    // via test-only macros to validate behavior in isolation.
    std::unique_ptr<Stmt> tryParseSpecialIdentifierStatement(const Token& startTok);
    std::unique_ptr<Stmt> tryParseDefFamily(const Token& startTok);
    std::unique_ptr<Stmt> tryParseGotoGosub(const Token& startTok);
    std::unique_ptr<Stmt> tryParseOtherKeywords(const Token& startTok);
    // Handle LET MID$(...) = expr$ case; returns nullptr if not matched
    std::unique_ptr<Stmt> parseMidAssignInLet(const Token& startTok);
    // Handle array element assignment tail after identifier and '('
    std::unique_ptr<Stmt> parseArrayAssignTail(const std::string& name, int lineNum, int colNum);
    /** Parse ON <expr> GOTO line[,line...] or ON <expr> GOSUB line[,line...] */
    std::unique_ptr<Stmt> parseOnGotoGosub();
    /** Parse ON ERROR GOTO (0|line) */
    std::unique_ptr<Stmt> parseOnErrorGoto();
    /**
     * Function: Parser::parsePrint
     * Purpose:
     *  - Parse a PRINT statement with comma/colon-separated items.
     * Outputs:
     *  - Stmt: PrintStmt node
     */
    std::unique_ptr<Stmt> parsePrint();
    /**
     * Function: Parser::parseLprint
     * Purpose:
     *  - Parse LPRINT (printer device) mirroring PRINT syntax, but uses
     *    the default printer channel. No '#n' prefix is allowed.
     * Outputs:
     *  - Stmt: PrintStmt node with channel implicitly set to printer.
     */
    std::unique_ptr<Stmt> parseLprint();
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
    std::unique_ptr<Stmt> parseRestore() const;
    /** Parse WRITE [#n,] expr[,expr...] */
    std::unique_ptr<Stmt> parseWrite();
    /** Parse ERROR numeric-expression */
    std::unique_ptr<Stmt> parseError();
    /** Parse RESUME [0|NEXT|line] */
    std::unique_ptr<Stmt> parseResume();
    /** Parse INPUT with optional #n, varlist or simple INPUT var */
    std::unique_ptr<Stmt> parseInput();
    /** Parse LINE INPUT [#n,] var$ */
    std::unique_ptr<Stmt> parseLineInput();
    /** Parse DEF FNname(param) = expression */
    std::unique_ptr<Stmt> parseDefFn();
    /** Parse DEFSTR/DEFINT/DEFSNG/DEFDBL letter range list */
    std::unique_ptr<Stmt> parseDefType(DefTypeStmt::Kind kind);
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
    /** Parse COLOR [fg][,[bg][,[border]]] */
    std::unique_ptr<Stmt> parseColor();
    /** Parse CHDIR string-expr */
    std::unique_ptr<Stmt> parseChdir();
    /** Parse SCREEN [mode][,[colorswitch][,[apage][,vpage]]] */
    std::unique_ptr<Stmt> parseScreen();
    /** Parse LOCATE row[,col] */
    std::unique_ptr<Stmt> parseLocate();
    /** Parse WIDTH [device$,] columns */
    std::unique_ptr<Stmt> parseWidth();
    /** Parse FILES [device$,] [path_or_pattern$] */
    std::unique_ptr<Stmt> parseFiles();
    /** Parse DELETE [start][- [end]] */
    std::unique_ptr<Stmt> parseDelete();
    /** Parse LIST [start][- [end]] */
    std::unique_ptr<Stmt> parseList();
    /** Parse LLIST [start][- [end]] (printer) */
    std::unique_ptr<Stmt> parseLlist();
    /** Parse MKDIR path$ */
    std::unique_ptr<Stmt> parseMkdir();
    /** Parse RMDIR path$ */
    std::unique_ptr<Stmt> parseRmdir();
    /** Parse KILL filespec$ */
    std::unique_ptr<Stmt> parseKill();
    /** Parse NAME old$ AS new$ */
    std::unique_ptr<Stmt> parseName();
    /** Parse SHELL [cmd$] */
    std::unique_ptr<Stmt> parseShell();
    /** Parse ENVIRON spec$ */
    std::unique_ptr<Stmt> parseEnviron();
    /** Parse CIRCLE x, y, r */
    std::unique_ptr<Stmt> parseCircle();
    /** Parse CLEAR */
    std::unique_ptr<Stmt> parseClear() const;
    /** Parse ERASE array[,array...] */
    std::unique_ptr<Stmt> parseErase();
    /** Parse SWAP varref, varref */
    std::unique_ptr<Stmt> parseSwap();
    /** Parse OPTION BASE 0|1 */
    std::unique_ptr<Stmt> parseOptionBase();
    /** Parse OPTION PRINTZONES ON|OFF */
    std::unique_ptr<Stmt> parseOptionPrintZones();
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
    /** Parse logical expressions (AND/OR) over comparison expressions. */
    std::unique_ptr<Expr> parseLogical();
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
     * Function: Parser::parsePower
     * Purpose:
     *  - Parse exponentiation (^) with right associativity.
     * Outputs:
     *  - Expr: Expression node
     */
    std::unique_ptr<Expr> parsePower();
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
    static const char* nodeName(const Stmt* stmt);

    /*
     * Property: sourcePath_
     * Purpose:
     *  - Base filesystem path for resolving relative filenames in MERGE,
     *    RUN, and CHAIN commands.
     * Notes:
     *  - Typically set when compiling from a file, empty when compiling
     *    from a raw string.
     */
    std::string sourcePath_;
};

} // namespace gwbasic

#endif // BASIC_COMPILER_PARSER_H
// NOLINTEND(llvm-header-guard)
