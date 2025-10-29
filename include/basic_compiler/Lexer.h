// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <fstream>
#include "basic_compiler/token/Token.h"

namespace gwbasic {

/**
 * LexError: Exception for lexical analysis failures.
 *
 * Purpose:
 *  - Signal unexpected characters or malformed literals.
 *
 * Inputs:
 *  - std::runtime_error::what(): error message.
 *
 * Outputs:
 *  - Exception object thrown by Lexer.
 */
class LexError final : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

/**
 * Lexer: Converts a GW-BASIC source string into a token stream.
 *
 * Purpose:
 *  - Perform lexical analysis required by the parser.
 */
class Lexer {
public:
    /**
     * Construct a lexer.
     *
     * Inputs:
     *  - source: Entire GW-BASIC program as a string.
     */
    explicit Lexer(std::string source)
        : src_(std::move(source)) {}

    /**
     * Tokenize: Produce the complete list of tokens for the source.
     *
     * Inputs:
     *  - none (uses source passed at construction).
     *
     * Outputs:
     *  - std::vector<Token>: Tokens including NewLine and a final EndOfFile.
     *
     * Purpose:
     *  - Feed the parser with a linear token stream.
     */
    std::vector<Token> tokenize();

    /**
     * setLexLogPath: Enable lexical logging to a file.
     *
     * Inputs:
     *  - path: Filesystem path to write token stream diagnostics.
     *
     * Outputs:
     *  - void (opens/initializes internal log file state)
     *
     * Purpose:
     *  - When enabled, each produced token is logged as: 
     *    "token <TYPE> @ <line>:<col> \"<lexeme>\"".
     */
    void setLexLogPath(const std::string& path);

private:
    std::string src_{};
    size_t pos_{0};
    int line_{1};
    int col_{1};
    bool bol_{true}; // beginning of line (before optional line number)

    bool atEnd() const { return pos_ >= src_.size(); }

    char peek() const { return atEnd() ? '\0' : src_[pos_]; }

    char peekNext() const { return (pos_ + 1 < src_.size()) ? src_[pos_ + 1] : '\0'; }

    /** advance: Consume and return the current character. */
    char advance();

    /** skipWhitespace: Skip spaces, tabs and comments to next significant char. */
    void skipWhitespace();

    /** number: Scan integer or floating-point literal. */
    Token number();

    /** identifierOrKeyword: Scan identifier or recognized keyword. */
    Token identifierOrKeyword();

    /** stringLiteral: Scan a double-quoted string with simple escapes. */
    Token stringLiteral();

    /** skipToEOL: Skip remaining characters until end-of-line. */
    void skipToEOL();

    // Lexical logging
    bool lexLogEnabled_{false};
    std::ofstream lexLog_;

    /** Log a token to the lex log if enabled. */
    void logToken(const Token& t);

    /** Escape text for readable logging. */
    static std::string escapeForLog(const std::string& s);
};

} // namespace gwbasic
