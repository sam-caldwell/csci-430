// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include <cctype>

namespace gwbasic {

/*
 * Function: Parser::tryParseDefFamily
 * Summary:
 *  Handle DEF family statements (SEG/USR/FN and range directives).
 * Parameters:
 *  - startTok: Token at which the statement begins for position assignment
 * Returns:
 *  - std::unique_ptr<Stmt>: Parsed DEF* node or nullptr if not matched
 */
std::unique_ptr<Stmt> Parser::tryParseDefFamily(const Token& startTok) {
    // Handle compound DEF* forms first (DEFINT/DEFSNG/DEFDBL/DEFSTR)
    if (match(TokenType::KwDefStr)) { auto n = parseDefType(DefTypeStmt::Kind::Str); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwDefInt)) { auto n = parseDefType(DefTypeStmt::Kind::Int); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwDefSng)) { auto n = parseDefType(DefTypeStmt::Kind::Sng); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwDefDbl)) { auto n = parseDefType(DefTypeStmt::Kind::Dbl); n->pos = {startTok.line, startTok.col}; return n; }

    if (!match(TokenType::KwDef)) return nullptr;
    if (match(TokenType::KwSeg)) { auto n = parseDefSeg(); n->pos = {startTok.line, startTok.col}; return n; }
    if (check(TokenType::Identifier)) {
        const std::string id = peek().lexeme; std::string up = id;
        for (auto &ch: up) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
        if (up.rfind("USR", 0) == 0) { auto n = parseDefUsr(); n->pos = {startTok.line, startTok.col}; return n; }
    }
    auto n = parseDefFn(); n->pos = {startTok.line, startTok.col}; return n;
}

} // namespace gwbasic
