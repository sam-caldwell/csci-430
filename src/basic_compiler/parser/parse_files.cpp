// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/FilesStmt.h"
#include "basic_compiler/ast/make_node.h"

namespace gwbasic {

/*
 * Function: Parser::parseFiles
 * Purpose:
 *  - Parse FILES [device$,] [path_or_pattern$]
 * Inputs:
 *  - none (assumes 'FILES' already matched by caller)
 * Outputs:
 *  - FilesStmt with optional device and pattern expressions.
 */
std::unique_ptr<Stmt> Parser::parseFiles() {
    const int l = peek().line, c = peek().col;
    // End of statement: no args
    if (check(TokenType::NewLine) || check(TokenType::Colon) || check(TokenType::EndOfFile)) {
        return make_node<FilesStmt>({l, c}, nullptr, nullptr);
    }
    auto first = parseExpression();
    std::unique_ptr<Expr> device;
    std::unique_ptr<Expr> pattern;
    if (match(TokenType::Comma)) {
        device = std::move(first);
        // pattern is optional; if EOL, leave null
        if (!(check(TokenType::NewLine) || check(TokenType::Colon) || check(TokenType::EndOfFile))) {
            pattern = parseExpression();
        }
    } else {
        pattern = std::move(first);
    }
    return make_node<FilesStmt>({l, c}, std::move(device), std::move(pattern));
}

} // namespace gwbasic

