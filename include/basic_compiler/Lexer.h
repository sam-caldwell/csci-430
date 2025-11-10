#ifndef BASIC_COMPILER_LEXER_H
#define BASIC_COMPILER_LEXER_H
// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <array>
#include <cstddef>
#include <istream>
#include <ostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>


#include "basic_compiler/Symbol.h"
#include "basic_compiler/token/Token.h"
#include "basic_compiler/token/TokenType.h"
#include "logger/Logger.h"

namespace gwbasic {

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
    explicit Lexer(std::string source);

    /**
     * Construct a lexer from an input stream.
     *
     * Inputs:
     *  - in: std::istream providing source characters (ifstream, stringstream, etc.)
     */
    explicit Lexer(std::istream& inputStream);

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
     * Keyword spellings used in special cases
     */
    static constexpr std::string_view KW_REM = "REM";
    /**
     * Compile-time keyword table (REM is handled specially as a comment)
     */
    static constexpr auto kKeywords_ = std::to_array<std::pair<std::string_view, TokenType>>({
        // REM intentionally omitted (treated as comment)
        std::pair{"ALL",       TokenType::KwAll},
        std::pair{"AND",       TokenType::KwAnd},
        std::pair{"AS",        TokenType::KwAs},
        std::pair{"BASE",      TokenType::KwBase},
        std::pair{"BLOAD",     TokenType::KwBload},
        std::pair{"BSAVE",     TokenType::KwBsave},
        std::pair{"CALL",      TokenType::KwCall},
        std::pair{"CHAIN",     TokenType::KwChain},
        std::pair{"CHDIR",     TokenType::KwChdir},
        std::pair{"CLEAR",     TokenType::KwClear},
        std::pair{"CLOSE",     TokenType::KwClose},
        std::pair{"COLOR",     TokenType::KwColor},
        std::pair{"COMMON",    TokenType::KwCommon},
        std::pair{"DATA",      TokenType::KwData},
        std::pair{"DEF",       TokenType::KwDef},
        std::pair{"DEFINT",    TokenType::KwDefInt},
        std::pair{"DEFSNG",    TokenType::KwDefSng},
        std::pair{"DEFDBL",    TokenType::KwDefDbl},
        std::pair{"DEFSTR",    TokenType::KwDefStr},
        std::pair{"DIM",       TokenType::KwDim},
        std::pair{"ELSE",      TokenType::KwElse},
        std::pair{"END",       TokenType::KwEnd},
        std::pair{"ERASE",     TokenType::KwErase},
        std::pair{"ERROR",     TokenType::KwError},
        std::pair{"FOR",       TokenType::KwFor},
        std::pair{"GOSUB",     TokenType::KwGosub},
        std::pair{"GOTO",      TokenType::KwGoto},
        std::pair{"FILES",     TokenType::KwFiles},
        std::pair{"NAME",      TokenType::KwName},
        std::pair{"KILL",      TokenType::KwKill},
        std::pair{"MKDIR",     TokenType::KwMkdir},
        std::pair{"RMDIR",     TokenType::KwRmdir},
        std::pair{"WIDTH",     TokenType::KwWidth},
        std::pair{"LOCATE",    TokenType::KwLocate},
        std::pair{"CLS",       TokenType::KwCls},
        std::pair{"PSET",      TokenType::KwPset},
        std::pair{"PRESET",    TokenType::KwPreset},
        std::pair{"PAINT",     TokenType::KwPaint},
        std::pair{"DRAW",      TokenType::KwDraw},
        std::pair{"VIEW",      TokenType::KwView},
        std::pair{"WINDOW",    TokenType::KwWindow},
        std::pair{"BEEP",      TokenType::KwBeep},
        std::pair{"LPRINT",    TokenType::KwLprint},
        std::pair{"SOUND",     TokenType::KwSound},
        std::pair{"PLAY",      TokenType::KwPlay},
        std::pair{"KEY",       TokenType::KwKey},
        std::pair{"PEN",       TokenType::KwPen},
        std::pair{"STRIG",     TokenType::KwStrig},
        std::pair{"TIMER",     TokenType::KwTimer},
        std::pair{"TRON",      TokenType::KwTron},
        std::pair{"TROFF",     TokenType::KwTroff},
        std::pair{"CONT",      TokenType::KwCont},
        std::pair{"LOAD",      TokenType::KwLoad},
        std::pair{"SAVE",      TokenType::KwSave},
        std::pair{"NEW",       TokenType::KwNew},
        std::pair{"DELETE",    TokenType::KwDelete},
        std::pair{"LIST",      TokenType::KwList},
        std::pair{"LLIST",     TokenType::KwLlist},
        std::pair{"AUTO",      TokenType::KwAuto},
        std::pair{"RENUM",     TokenType::KwRenum},
        std::pair{"EDIT",      TokenType::KwEdit},
        std::pair{"PCOPY",     TokenType::KwPcopy},
        std::pair{"RESET",     TokenType::KwReset},
        std::pair{"SHELL",     TokenType::KwShell},
        std::pair{"ENVIRON",   TokenType::KwEnviron},
        std::pair{"OUT",       TokenType::KwOut},
        std::pair{"WAIT",      TokenType::KwWait},
        std::pair{"IF",        TokenType::KwIf},
        std::pair{"INPUT",     TokenType::KwInput},
        std::pair{"LET",       TokenType::KwLet},
        std::pair{"LINE",      TokenType::KwLine},
        std::pair{"MERGE",     TokenType::KwMerge},
        std::pair{"MOD",       TokenType::KwMod},
        std::pair{"NEXT",      TokenType::KwNext},
        std::pair{"NOT",       TokenType::KwNot},
        std::pair{"ON",        TokenType::KwOn},
        std::pair{"OPEN",      TokenType::KwOpen},
        std::pair{"OPTION",    TokenType::KwOption},
        std::pair{"OR",        TokenType::KwOr},
        std::pair{"OUTPUT",    TokenType::KwOutput},
        std::pair{"POKE",      TokenType::KwPoke},
        std::pair{"PRINT",     TokenType::KwPrint},
        std::pair{"RANDOMIZE", TokenType::KwRandomize},
        std::pair{"READ",      TokenType::KwRead},
        std::pair{"RESTORE",   TokenType::KwRestore},
        std::pair{"RESUME",    TokenType::KwResume},
        std::pair{"RETURN",    TokenType::KwReturn},
        std::pair{"RUN",       TokenType::KwRun},
        std::pair{"SEG",       TokenType::KwSeg},
        std::pair{"STEP",      TokenType::KwStep},
        std::pair{"STOP",      TokenType::KwStop},
        std::pair{"SWAP",      TokenType::KwSwap},
        std::pair{"SYSTEM",    TokenType::KwSystem},
        std::pair{"THEN",      TokenType::KwThen},
        std::pair{"TO",        TokenType::KwTo},
        std::pair{"USING",     TokenType::KwUsing},
        std::pair{"WEND",      TokenType::KwWend},
        std::pair{"WHILE",     TokenType::KwWhile},
        std::pair{"WRITE",     TokenType::KwWrite},
    });

    /*
     * Function: Lexer::lookupKeyword
     * Purpose:
     *  - Map an uppercase identifier to a keyword token type.
     * Inputs:
     *  - upper: Uppercase candidate identifier
     * Outputs:
     *  - TokenType: Matching keyword type, or Identifier if not matched.
     */
    static TokenType lookupKeyword(std::string_view upper);

    /*
     * Template: Lexer::scanWhile
     * Purpose:
     *  - Accumulate characters while a predicate over the current char holds,
     *    consuming input via advance().
     * Inputs:
     *  - pred(char)->bool: predicate deciding whether to continue scanning
     * Outputs:
     *  - std::string: collected characters
     */
    template <class Pred>
    std::string scanWhile(Pred&& pred) {
        std::string out;
        while (!atEnd() && std::forward<Pred>(pred)(peek())) {
            out.push_back(advance());
        }
        return out;
    }

    /*
     * Template: Lexer::skipWhile
     * Purpose:
     *  - Consume characters while a predicate over the current char holds.
     * Inputs:
     *  - pred(char)->bool: predicate deciding whether to continue skipping
     * Outputs:
     *  - void (advances internal cursor)
     */
    template <class Pred>
    void skipWhile(Pred&& pred) {
        while (!atEnd() && std::forward<Pred>(pred)(peek())) {
            advance();
        }
    }

    /*
     * Template: Lexer::emitFixed
     * Purpose:
     *  - Construct, append and log a token with a fixed lexeme.
     * Inputs:
     *  - out: token destination vector
     *  - lex: fixed lexeme as a null-terminated char array
     *  - line/col: source position to assign
     * Outputs:
     *  - void (pushes token and logs it)
     */
    // Note: string_view overload preferred to avoid C-style arrays.

    // Overload: emitFixed from string_view
    template <TokenType TT>
    void emitFixed(std::vector<Token>& out, std::string_view lex, const int line, const int col) {
        Token token{TT, std::string(lex), line, col};
        out.emplace_back(token);
        logToken(token);
    }

    // Overload: emitFixed from Symbol
    template <TokenType TT>
    void emitFixed(std::vector<Token>& out, const Symbol& sym, const int line, const int col) {
        Token token{TT, sym.to_string(), line, col};
        out.emplace_back(token);
        logToken(token);
    }

    /*
     * Function: Lexer::emitToken
     * Purpose:
     *  - Push a pre-constructed token into the output vector and log it.
     * Inputs:
     *  - out: token destination vector
     *  - t: token to emit (by const ref)
     * Outputs:
     *  - void (pushes token and logs it)
     */
    void emitToken(std::vector<Token>& out, const Token& token);

    /*
     * Template: Lexer::emitPairOrSingle
     * Purpose:
     *  - After consuming the first character of an operator, emit a two-char
     *    operator when the next character matches, otherwise emit the single
     *    operator.
     * Inputs:
     *  - out: token destination vector
     *  - singleLex/pairLex: lexemes for single and pair operators
     *  - line/col: source position to assign
     * Notes:
     *  - Assumes caller already consumed the first character with advance().
     */
    template <TokenType Single, TokenType Pair, char Next>
    // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    void emitPairOrSingle(std::vector<Token>& out,
                          std::array<std::string_view, 2> lexemes,
                          const int line, const int col) {

        if (peek() == Next) {
            advance();
            emitFixed<Pair>(out, lexemes[1], line, col);
        } else {
            emitFixed<Single>(out, lexemes[0], line, col);
        }
    }

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
     *  - Reset to 1 on the newline; incremented on other characters.
     */
    int col_{1};

    /*
     * Property: bol_
     * Purpose:
     *  - Beginning-of-line indicator (true before the first non-newline char).
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
    bool atEnd() const;

    /*
     * Function: Lexer::peek
     * Purpose:
     *  - Inspect the current character without consuming it.
     * Inputs:
     *  - none (uses internal cursor state)
     * Outputs:
     *  - char: current character or '\0' at end-of-input
     */
    char peek() const;

    /*
     * Function: Lexer::peekNext
     * Purpose:
     *  - Look one character ahead without consuming.
     * Inputs:
     *  - none (uses internal cursor state)
     * Outputs:
     *  - char: next character or '\0' if beyond end-of-input
     */
    char peekNext() const;

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
     *  - Scan a double-quoted string literal using GW-BASIC rules.
     * Inputs:
     *  - none
     * Outputs:
     *  - Token: String token with content; doubled quotes ("") decode to
     *           a single double quote. No C-style escapes are processed.
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

    // Lexical logging via ostream-based logger
    logger::Logger lexLogger_{};

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
    void logToken(const Token& token);
    // Stream accessor for lex logging
    std::ostream& log();

    /*
     * Function: Lexer::escapeForLog
     * Purpose:
     *  - Escape control characters and quotes for readable log output.
     * Inputs:
     *  - s: Raw string input
     * Outputs:
     *  - std::string: Escaped representation suitable for logs
     */
    static std::string escapeForLog(const std::string& text);

    // Lightweight helpers used by tokenize() to reduce branching
    bool tryEmitNewline(std::vector<Token>& out);
    bool tryEmitPrimary(std::vector<Token>& out);
    void emitAmpLiteral(std::vector<Token>& out, int line, int col);
    bool tryEmitOperatorOrPunct(std::vector<Token>& out, int line, int col, char chr);

    // Friend accessor for tests: exposes a minimal surface to validate
    // internal cursor movement semantics without widening the public API.
    friend class LexerAccessorForTests;
};

// Minimal friend accessor for tests: allows calling advance() and
// inspecting cursor state (line/col/bol) and peek/atEnd for verification.
class LexerAccessorForTests {
public:
    static char advance(Lexer& lexer) { return lexer.advance(); }
    static int line(const Lexer& lexer) { return lexer.line_; }
    static int col(const Lexer& lexer) { return lexer.col_; }
    static bool bol(const Lexer& lexer) { return lexer.bol_; }
    static bool atEnd(const Lexer& lexer) { return lexer.atEnd(); }
    static char peek(const Lexer& lexer) { return lexer.peek(); }
    static void emitAmpLiteral(Lexer& lexer, std::vector<Token>& out, int line, int col) { lexer.emitAmpLiteral(out, line, col); }
    static std::string escapeForLog(const std::string& text) { return Lexer::escapeForLog(text); }
    static Token identifierOrKeyword(Lexer& lexer) { return lexer.identifierOrKeyword(); }
};

} // namespace gwbasic

#endif // BASIC_COMPILER_LEXER_H
