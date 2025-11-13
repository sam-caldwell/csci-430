// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/PrintStmt.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/parser/ParseError.h"
#include "basic_compiler/token/TokenType.h"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace gwbasic {

/*
 * Function: Parser::parsePrint
 * Summary:
 *  Parse PRINT (and variants) into a PrintStmt with items and separators.
 * Parameters:
 *  - none
 * Returns:
 *  - std::unique_ptr<Stmt>: PrintStmt capturing items, seps, format, channel
 */
std::unique_ptr<Stmt> Parser::parsePrint() { // NOLINT(readability-function-size,readability-function-cognitive-complexity)
    const int lineNum = peek().line;
    const int colNum = peek().col;
    int channel = -1;
    // Optional: PRINT # n ,
    if (match(TokenType::Hash)) {
        if (!check(TokenType::Integer)) {
            throw ParseError("Expected channel number after '#'");
        }
        channel = std::stoi(peek().lexeme);
        advance();
        if (match(TokenType::Comma)) {
            // ok
        }
    }
    // Optional: USING formatExpr ; or , (leading)
    std::unique_ptr<Expr> fmt;
    if (match(TokenType::KwUsing)) {
        fmt = parseExpression();
        // Historically PRINT USING requires ';' before the value list, but
        // we accept either ';' or ',' for compatibility.
        (void)(match(TokenType::Semicolon) || match(TokenType::Comma));
    }
    // Support degenerate forms: PRINT ; / PRINT , (no items)
    if (match(TokenType::Semicolon)) {
        auto node = make_node<PrintStmt>({lineNum, colNum}, std::vector<std::unique_ptr<Expr>>{});
        node->channel = channel;
        node->format = std::move(fmt);
        node->trail = PrintStmt::Terminator::Semicolon;
        return node;
    }
    if (match(TokenType::Comma)) {
        auto node = make_node<PrintStmt>({lineNum, colNum}, std::vector<std::unique_ptr<Expr>>{});
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
    PrintStmt::Terminator trail = PrintStmt::Terminator::Newline;
    while (true) {
        // Mid-list USING: update format and continue without consuming a value
        if (check(TokenType::KwUsing)) {
            advance();
            fmt = parseExpression();
            // Optional separator after USING; ignore for item emission
            (void)(match(TokenType::Semicolon) || match(TokenType::Comma));
            // Continue to accept next item or another USING
            continue;
        }
        // If the next token begins an expression, parse it; otherwise, break
        if (check(TokenType::String) || check(TokenType::Integer) || check(TokenType::Float) ||
            check(TokenType::Identifier) || check(TokenType::LParen) || check(TokenType::Plus) ||
            check(TokenType::Minus) || check(TokenType::KwNot)) {
            items.push_back(parseExpression());
        } else {
            break;
        }
        // After an item, capture a separator if present and loop for the next
        if (match(TokenType::Comma)) {
            // If end-of-list follows, treat as trailing terminator
            if (check(TokenType::NewLine) || check(TokenType::Colon) || check(TokenType::EndOfFile)) {
                trail = PrintStmt::Terminator::Comma;
                break;
            }
            seps.push_back(PrintStmt::Sep::Comma);
            continue;
        }
        if (match(TokenType::Semicolon)) {
            if (check(TokenType::NewLine) || check(TokenType::Colon) || check(TokenType::EndOfFile)) {
                trail = PrintStmt::Terminator::Semicolon;
                break;
            }
            seps.push_back(PrintStmt::Sep::Semicolon);
            continue;
        }
        break;
    }
    auto node = make_node<PrintStmt>({lineNum, colNum}, std::move(items));
    node->seps = std::move(seps);
    node->channel = channel;
    node->format = std::move(fmt);
    // Trailing terminator inherited from loop detection (default newline)
    node->trail = trail;
    return node;
}

} // namespace gwbasic
