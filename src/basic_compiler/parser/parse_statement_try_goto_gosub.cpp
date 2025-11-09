// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/GosubStmt.h"
#include "basic_compiler/ast/GotoStmt.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/parser/ParseError.h"
#include "basic_compiler/token/Token.h"
#include "basic_compiler/token/TokenType.h"
#include <memory>
#include <string>

namespace gwbasic {

/*
 * Function: Parser::tryParseGotoGosub
 * Inputs:
 *  - startTok: Starting token for position metadata
 * Outputs:
 *  - GotoStmt or GosubStmt on success; nullptr when neither keyword matches.
 * Theory of operation:
 *  - Consumes GOTO/GOSUB followed by a required integer line number.
 */
std::unique_ptr<Stmt> Parser::tryParseGotoGosub(const Token& startTok) {
    if (match(TokenType::KwGoto)) {
        if (!check(TokenType::Integer)) {
            throw ParseError("Expected line number after GOTO");
        }
        const int target = std::stoi(peek().lexeme);
        advance();
        return make_node<GotoStmt>({startTok.line, startTok.col}, target);
    }
    if (match(TokenType::KwGosub)) {
        if (!check(TokenType::Integer)) {
            throw ParseError("Expected line number after GOSUB");
        }
        const int target = std::stoi(peek().lexeme);
        advance();
        return make_node<GosubStmt>({startTok.line, startTok.col}, target);
    }
    return nullptr;
}

} // namespace gwbasic
