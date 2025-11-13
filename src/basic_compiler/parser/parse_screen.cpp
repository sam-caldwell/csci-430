// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/ScreenStmt.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/token/TokenType.h"
#include <memory>
#include <utility>

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
std::unique_ptr<Stmt> Parser::parseScreen() { // NOLINT(readability-function-size,readability-function-cognitive-complexity)
    std::unique_ptr<Expr> mode;
    std::unique_ptr<Expr> colorSwitch;
    std::unique_ptr<Expr> activePage;
    std::unique_ptr<Expr> videoPage;
    // If the next token begins an expression and is not a separator, parse it
    if (!(check(TokenType::NewLine) || check(TokenType::Colon) || check(TokenType::EndOfFile) || check(TokenType::Comma))) {
        mode = parseExpression();
    }
    if (match(TokenType::Comma)) {
        if (!(check(TokenType::NewLine) || check(TokenType::Colon) || check(TokenType::EndOfFile) || check(TokenType::Comma))) {
            colorSwitch = parseExpression();
        }
        if (match(TokenType::Comma)) {
            if (!(check(TokenType::NewLine) || check(TokenType::Colon) || check(TokenType::EndOfFile) || check(TokenType::Comma))) {
                activePage = parseExpression();
            }
            if (match(TokenType::Comma)) {
                if (!(check(TokenType::NewLine) || check(TokenType::Colon) || check(TokenType::EndOfFile))) {
                    videoPage = parseExpression();
                }
            }
        }
    }
    return make_node<ScreenStmt>({0,0}, std::move(mode), std::move(colorSwitch), std::move(activePage), std::move(videoPage));
}

} // namespace gwbasic
