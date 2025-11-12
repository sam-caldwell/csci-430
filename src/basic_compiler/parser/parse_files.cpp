// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/FilesStmt.h"
#include "basic_compiler/ast/make_node.h"

namespace gwbasic {

/*
 * Function: Parser::parseFiles
 * Summary:
 *  Parse FILES [device$,] [path_or_pattern$] into a FilesStmt.
 * Parameters:
 *  - none
 * Returns:
 *  - std::unique_ptr<Stmt>: FilesStmt with optional device and pattern
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
