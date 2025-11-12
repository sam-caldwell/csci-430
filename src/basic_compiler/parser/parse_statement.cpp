// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/DefTypeStmt.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/parser/ParseError.h"
#include "basic_compiler/token/ToString.h"
#include "basic_compiler/token/Token.h"
#include "basic_compiler/token/TokenType.h"
#include <memory>
#include <sstream>

namespace gwbasic {

/*
 * Function: Parser::parseStatement
 * Summary:
 *  Dispatch on the current token to parse a single statement.
 * Parameters:
 *  - none
 * Returns:
 *  - std::unique_ptr<Stmt>: Parsed statement node
 */
std::unique_ptr<Stmt> Parser::parseStatement() {
    const Token startTok = peek();
    if (auto parsed = tryParseSpecialIdentifierStatement(startTok)) {
        return parsed;
    }
    if (auto parsed = tryParseDefFamily(startTok)) {
        return parsed;
    }
    // Also handle DEF* range directives explicitly for robustness
    if (match(TokenType::KwDefStr)) {
        auto defNode = parseDefType(DefTypeStmt::Kind::Str);
        defNode->pos = {startTok.line, startTok.col};
        return defNode;
    }
    if (match(TokenType::KwDefInt)) {
        auto defNode = parseDefType(DefTypeStmt::Kind::Int);
        defNode->pos = {startTok.line, startTok.col};
        return defNode;
    }
    if (match(TokenType::KwDefSng)) {
        auto defNode = parseDefType(DefTypeStmt::Kind::Sng);
        defNode->pos = {startTok.line, startTok.col};
        return defNode;
    }
    if (match(TokenType::KwDefDbl)) {
        auto defNode = parseDefType(DefTypeStmt::Kind::Dbl);
        defNode->pos = {startTok.line, startTok.col};
        return defNode;
    }
    if (auto parsed = tryParseGotoGosub(startTok)) {
        return parsed;
    }
    if (auto parsed = tryParseOtherKeywords(startTok)) {
        return parsed;
    }
    if (check(TokenType::KwLet) || check(TokenType::Identifier)) {
        auto stmtNode = parseAssignOrLet();
        stmtNode->pos = {startTok.line, startTok.col};
        return stmtNode;
    }
    std::ostringstream oss;
    oss << "Unexpected token in statement: " << to_string(peek().type) << " at " << peek().line << ":" << peek().col;
    throw ParseError(oss.str());
}

} // namespace gwbasic
