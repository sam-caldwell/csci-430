// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/PrintStmt.h"

namespace gwbasic {

/*
 * Function: Parser::parseLprint
 * Purpose:
 *  - Parse LPRINT [USING fmt][;|,] items...
 *    Same as PRINT but without optional '#n' channel prefix; implicitly
 *    targets the default printer channel.
 */
// NOLINTNEXTLINE(readability-function-cognitive-complexity,bugprone-branch-clone)
std::unique_ptr<Stmt> Parser::parseLprint() {
    const int l = peek().line;
    const int c = peek().col;
    int channel = 1; // Implicit printer channel maps to @gwb_files[0]
    // Optional: USING formatExpr ; or , (leading)
    std::unique_ptr<Expr> fmt;
    if (match(TokenType::KwUsing)) {
        fmt = parseExpression();
        (void)(match(TokenType::Semicolon) || match(TokenType::Comma));
    }
    // Degenerate: LPRINT ; or LPRINT , (no items)
    if (match(TokenType::Semicolon)) {
        auto node = make_node<PrintStmt>({l, c}, std::vector<std::unique_ptr<Expr>>{});
        node->channel = channel;
        node->format = std::move(fmt);
        node->trail = PrintStmt::Terminator::Semicolon;
        return node;
    }
    if (match(TokenType::Comma)) {
        auto node = make_node<PrintStmt>({l, c}, std::vector<std::unique_ptr<Expr>>{});
        node->channel = channel;
        node->format = std::move(fmt);
        node->trail = PrintStmt::Terminator::Comma;
        return node;
    }
    std::vector<std::unique_ptr<Expr>> items;
    std::vector<PrintStmt::Sep> seps;
    PrintStmt::Terminator trail = PrintStmt::Terminator::Newline;
    while (true) {
        if (check(TokenType::KwUsing)) {
            advance();
            fmt = parseExpression();
            (void)(match(TokenType::Semicolon) || match(TokenType::Comma));
            continue;
        }
        if (check(TokenType::String) || check(TokenType::Integer) || check(TokenType::Float) || check(TokenType::Identifier) || check(TokenType::LParen) || check(TokenType::Plus) || check(TokenType::Minus) || check(TokenType::KwNot)) {
            items.push_back(parseExpression());
        } else {
            break;
        }
        if (match(TokenType::Comma)) {
            if (check(TokenType::NewLine) || check(TokenType::Colon) || check(TokenType::EndOfFile)) { trail = PrintStmt::Terminator::Comma; break; }
            seps.push_back(PrintStmt::Sep::Comma);
            continue;
        }
        if (match(TokenType::Semicolon)) {
            if (check(TokenType::NewLine) || check(TokenType::Colon) || check(TokenType::EndOfFile)) { trail = PrintStmt::Terminator::Semicolon; break; }
            seps.push_back(PrintStmt::Sep::Semicolon);
            continue;
        }
        break;
    }
    auto node = make_node<PrintStmt>({l, c}, std::move(items));
    node->seps = std::move(seps);
    node->channel = channel;
    node->format = std::move(fmt);
    node->trail = trail;
    return node;
}

} // namespace gwbasic
