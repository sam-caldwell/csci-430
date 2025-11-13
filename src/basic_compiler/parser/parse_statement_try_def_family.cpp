// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/DefTypeStmt.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/token/Token.h"
#include "basic_compiler/token/TokenType.h"
#include <cctype>
#include <memory>
#include <string>

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
    if (match(TokenType::KwDefStr)) {
        auto node = parseDefType(DefTypeStmt::Kind::Str);
        node->pos = {startTok.line, startTok.col};
        return node;
    }
    if (match(TokenType::KwDefInt)) {
        auto node = parseDefType(DefTypeStmt::Kind::Int);
        node->pos = {startTok.line, startTok.col};
        return node;
    }
    if (match(TokenType::KwDefSng)) {
        auto node = parseDefType(DefTypeStmt::Kind::Sng);
        node->pos = {startTok.line, startTok.col};
        return node;
    }
    if (match(TokenType::KwDefDbl)) {
        auto node = parseDefType(DefTypeStmt::Kind::Dbl);
        node->pos = {startTok.line, startTok.col};
        return node;
    }

    if (!match(TokenType::KwDef)) {
        return nullptr;
    }
    if (match(TokenType::KwSeg)) {
        auto node = parseDefSeg();
        node->pos = {startTok.line, startTok.col};
        return node;
    }
    if (check(TokenType::Identifier)) {
        const std::string ident = peek().lexeme;
        std::string upper = ident;
        for (auto &chr: upper) {
            chr = static_cast<char>(std::toupper(static_cast<unsigned char>(chr)));
        }
        if (upper.rfind("USR", 0) == 0) {
            auto node = parseDefUsr();
            node->pos = {startTok.line, startTok.col};
            return node;
        }
    }
    auto node = parseDefFn();
    node->pos = {startTok.line, startTok.col};
    return node;
}

} // namespace gwbasic
