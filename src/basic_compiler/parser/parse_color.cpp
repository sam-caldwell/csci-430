// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/ColorStmt.h"
#include "basic_compiler/ast/make_node.h"

namespace gwbasic {

/*
 * Function: Parser::parseColor
 * Purpose:
 *  - Parse COLOR [fg][,[bg][,[border]]]
 * Inputs:
 *  - none (assumes 'COLOR' matched by caller)
 * Outputs:
 *  - ColorStmt: optional foreground, background, and border expressions
 */
std::unique_ptr<Stmt> Parser::parseColor() {
    // COLOR [fg][,[bg][,[border]]]
    std::unique_ptr<Expr> fg, bg, br;
    // If next token can start an expression and not a separator, parse it
    if (!(check(TokenType::NewLine) || check(TokenType::Colon) || check(TokenType::EndOfFile) || check(TokenType::Comma))) {
        fg = parseExpression();
    }
    if (match(TokenType::Comma)) {
        if (!(check(TokenType::NewLine) || check(TokenType::Colon) || check(TokenType::EndOfFile) || check(TokenType::Comma))) {
            bg = parseExpression();
        }
        if (match(TokenType::Comma)) {
            if (!(check(TokenType::NewLine) || check(TokenType::Colon) || check(TokenType::EndOfFile))) {
                br = parseExpression();
            }
        }
    }
    return make_node<ColorStmt>({0,0}, std::move(fg), std::move(bg), std::move(br));
}

} // namespace gwbasic
