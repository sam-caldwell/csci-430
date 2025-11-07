// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/PrintStmt.h"
#include "basic_compiler/ast/StringExpr.h"

namespace gwbasic {

/*
 * Function: Parser::parsePrint
 * Inputs:
 *  - none (assumes PRINT already consumed)
 * Outputs:
 *  - PrintStmt: printing either a string literal or an expression
 * Theory of operation:
 *  - If the next token is STRING, returns a PrintStmt with StringExpr;
 *    otherwise parses an expression and returns a PrintStmt for numeric
 *    output.
 */
std::unique_ptr<Stmt> Parser::parsePrint() {
    const int l = peek().line;
    const int c = peek().col;
    int channel = -1;
    // Optional: PRINT # n ,
    if (match(TokenType::Hash)) {
        if (!check(TokenType::Integer)) throw ParseError("Expected channel number after '#'");
        channel = std::stoi(peek().lexeme); advance();
        if (match(TokenType::Comma)) {}
    }
    // Optional: USING formatExpr ; or , (leading)
    std::unique_ptr<Expr> fmt;
    if (match(TokenType::KwUsing)) {
        fmt = parseExpression();
        // Historically PRINT USING requires ';' before the value list, but
        // we accept either ';' or ',' for compatibility.
        if (match(TokenType::Semicolon)) {
            // ok
        } else if (match(TokenType::Comma)) {
            // ok
        }
    }
    // Support degenerate forms: PRINT ; / PRINT , (no items)
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
    // Parse a list of items separated by commas/semicolons. Allow USING(fmt)
    // to appear mid-list; when encountered, set/replace the active format and
    // do not emit a value item for it. Last one wins.
    std::vector<std::unique_ptr<Expr>> items;
    std::vector<PrintStmt::Sep> seps;
    bool any = false;
    PrintStmt::Terminator trail = PrintStmt::Terminator::Newline;
    while (true) {
        // Mid-list USING: update format and continue without consuming a value
        if (check(TokenType::KwUsing)) {
            advance();
            fmt = parseExpression();
            // Optional separator after USING; ignore for item emission
            if (match(TokenType::Semicolon)) {}
            else if (match(TokenType::Comma)) {}
            // Continue to accept next item or another USING
            continue;
        }
        // If the next token begins an expression, parse it; otherwise, break
        if (check(TokenType::String) || check(TokenType::Integer) || check(TokenType::Float) || check(TokenType::Identifier) || check(TokenType::LParen) || check(TokenType::Plus) || check(TokenType::Minus) || check(TokenType::KwNot)) {
            items.push_back(parseExpression());
            any = true;
        } else {
            break;
        }
        // After an item, capture a separator if present and loop for the next
        if (match(TokenType::Comma)) {
            // If end-of-list follows, treat as trailing terminator
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
    // Trailing terminator inherited from loop detection (default newline)
    node->trail = trail;
    return node;
}

} // namespace gwbasic
