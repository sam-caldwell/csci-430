// NOLINTBEGIN(llvm-header-guard)
#ifndef BASIC_COMPILER_LEXERACCESSORFORTESTS_H
#define BASIC_COMPILER_LEXERACCESSORFORTESTS_H
// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <string>
#include <vector>

#include "basic_compiler/lexer/Lexer.h"
#include "basic_compiler/lexer/LexError.h" // for EXPECT_THROW in tests
#include "basic_compiler/token/Token.h"

namespace gwbasic {

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

#endif // BASIC_COMPILER_LEXERACCESSORFORTESTS_H
// NOLINTEND(llvm-header-guard)
