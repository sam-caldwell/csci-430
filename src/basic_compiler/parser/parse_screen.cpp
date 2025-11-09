// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/ScreenStmt.h"
#include "basic_compiler/ast/make_node.h"

namespace gwbasic {

/*
 * Function: Parser::parseScreen
 * Summary:
 *  Parse SCREEN [mode][,[colorswitch][,[apage][,vpage]]] into a ScreenStmt.
 * Parameters:
 *  - none
 * Returns:
 *  - std::unique_ptr<Stmt>: ScreenStmt with up to four optional expressions
 */
std::unique_ptr<Stmt> Parser::parseScreen() {
    std::unique_ptr<Expr> mode, cs, ap, vp;
    // If the next token begins an expression and is not a separator, parse it
    if (!(check(TokenType::NewLine) || check(TokenType::Colon) || check(TokenType::EndOfFile) || check(TokenType::Comma))) {
        mode = parseExpression();
    }
    if (match(TokenType::Comma)) {
        if (!(check(TokenType::NewLine) || check(TokenType::Colon) || check(TokenType::EndOfFile) || check(TokenType::Comma))) {
            cs = parseExpression();
        }
        if (match(TokenType::Comma)) {
            if (!(check(TokenType::NewLine) || check(TokenType::Colon) || check(TokenType::EndOfFile) || check(TokenType::Comma))) {
                ap = parseExpression();
            }
            if (match(TokenType::Comma)) {
                if (!(check(TokenType::NewLine) || check(TokenType::Colon) || check(TokenType::EndOfFile))) {
                    vp = parseExpression();
                }
            }
        }
    }
    return make_node<ScreenStmt>({0,0}, std::move(mode), std::move(cs), std::move(ap), std::move(vp));
}

} // namespace gwbasic
