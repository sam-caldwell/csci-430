// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <string>
#include <istream>
#include <sstream>
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
     * Construct a lexer from an input stream.
     *
     * Inputs:
     *  - in: std::istream providing source characters (ifstream, stringstream, etc.)
     */
    explicit Lexer(std::istream& in) {
        std::ostringstream buf;
        buf << in.rdbuf();
        src_ = std::move(buf).str();
    }

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
    /*
     * Property: src_
     * Purpose:
     *  - Holds the entire GW-BASIC source being lexed.
     * Notes:
     *  - The lexer operates over this immutable buffer using 'pos_'.
     */
    std::string src_{};

    /*
     * Property: pos_
     * Purpose:
     *  - Current byte offset into 'src_'.
     * Notes:
     *  - Zero-based; advanced by 'advance()' and helpers.
     */
    size_t pos_{0};

    /*
     * Property: line_
     * Purpose:
     *  - 1-based current line number for tokens and diagnostics.
     * Notes:
     *  - Incremented on '\n' by 'advance()'.
     */
    int line_{1};

    /*
     * Property: col_
     * Purpose:
     *  - 1-based current column within the current line.
     * Notes:
     *  - Reset to 1 on newline; incremented on other characters.
     */
    int col_{1};

    /*
     * Property: bol_
     * Purpose:
     *  - Beginning-of-line indicator (true before first non-newline char).
     * Notes:
     *  - Used for optional line number handling and diagnostics alignment.
     */
    bool bol_{true};

    /*
     * Function: Lexer::atEnd
     * Purpose:
     *  - Report whether the lexer has consumed all source characters.
     * Inputs:
     *  - none (uses internal cursor state)
     * Outputs:
     *  - bool: true when the cursor is at or beyond the last character
     */
    bool atEnd() const { return pos_ >= src_.size(); }

    /*
     * Function: Lexer::peek
     * Purpose:
     *  - Inspect the current character without consuming it.
     * Inputs:
     *  - none (uses internal cursor state)
     * Outputs:
     *  - char: current character or '\0' at end-of-input
     */
    char peek() const { return atEnd() ? '\0' : src_[pos_]; }

    /*
     * Function: Lexer::peekNext
     * Purpose:
     *  - Look one character ahead without consuming.
     * Inputs:
     *  - none (uses internal cursor state)
     * Outputs:
     *  - char: next character or '\0' if beyond end-of-input
     */
    char peekNext() const { return (pos_ + 1 < src_.size()) ? src_[pos_ + 1] : '\0'; }

    /*
     * Function: Lexer::advance
     * Purpose:
     *  - Consume and return the current character, updating line/column and
     *    beginning-of-line tracking on newlines.
     * Inputs:
     *  - none (uses internal cursor state)
     * Outputs:
     *  - char: the character consumed, or '\0' at end-of-input
     */
    char advance();

    /*
     * Function: Lexer::skipWhitespace
     * Purpose:
     *  - Advance past spaces, tabs, and comments until a significant
     *    character is found.
     * Inputs:
     *  - none
     * Outputs:
     *  - void (moves internal cursor)
     */
    void skipWhitespace();

    /*
     * Function: Lexer::number
     * Purpose:
     *  - Scan an integer or floating-point numeric literal starting at the
     *    current position.
     * Inputs:
     *  - none (reads from internal cursor)
     * Outputs:
     *  - Token: Integer or Float token with captured lexeme and position
     */
    Token number();

    /*
     * Function: Lexer::identifierOrKeyword
     * Purpose:
     *  - Scan an identifier and normalize it to a keyword token when the
     *    text matches a reserved word.
     * Inputs:
     *  - none
     * Outputs:
     *  - Token: Identifier or keyword token with position metadata
     */
    Token identifierOrKeyword();

    /*
     * Function: Lexer::stringLiteral
     * Purpose:
     *  - Scan a double-quoted string literal handling common escapes.
     * Inputs:
     *  - none
     * Outputs:
     *  - Token: String token with unescaped content and position
     */
    Token stringLiteral();

    /*
     * Function: Lexer::skipToEOL
     * Purpose:
     *  - Skip all remaining characters on the current line.
     * Inputs:
     *  - none
     * Outputs:
     *  - void (advances to newline or end-of-input)
     */
    void skipToEOL();

    // Lexical logging
    /*
     * Property: lexLogEnabled_
     * Purpose:
     *  - Controls whether lexical tokens are written to the log.
     * Notes:
     *  - Enabled via setLexLogPath(); false disables logging fast-path.
     */
    bool lexLogEnabled_{false};

    /*
     * Property: lexLog_
     * Purpose:
     *  - Output stream used to record tokenization events.
     * Notes:
     *  - Opened/truncated in setLexLogPath(); checked before writes.
     */
    std::ofstream lexLog_;

    /*
     * Function: Lexer::logToken
     * Purpose:
     *  - Emit a human-readable token description to the lex log when
     *    logging is enabled.
     * Inputs:
     *  - t: Token to log
     * Outputs:
     *  - void (writes a line to the log file if open)
     */
    void logToken(const Token& t);

    /*
     * Function: Lexer::escapeForLog
     * Purpose:
     *  - Escape control characters and quotes for readable log output.
     * Inputs:
     *  - s: Raw string input
     * Outputs:
     *  - std::string: Escaped representation suitable for logs
     */
    static std::string escapeForLog(const std::string& s);
};

} // namespace gwbasic
