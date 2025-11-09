// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/PrintStmt.h"

namespace gwbasic {

/*
 * Function: Parser::parseLprint
 * Summary:
 *  Parse LPRINT with optional USING, item list, and terminators.
 * Parameters:
 *  - none
 * Returns:
 *  - std::unique_ptr<Stmt>: PrintStmt targeting printer channel 1
 */
std::unique_ptr<Stmt> Parser::parseLprint() {
    const int l = peek().line;
    const int c = peek().col;
    constexpr int channel = 1; // Implicit printer channel maps to @gwb_files[0]

    auto makePrintNode = [&](std::vector<std::unique_ptr<Expr>> items,
                             PrintStmt::Terminator trail,
                             std::unique_ptr<Expr> fmtPtr) -> std::unique_ptr<PrintStmt> {
        auto node = make_node<PrintStmt>({l, c}, std::move(items));
        node->channel = channel;
        node->format = std::move(fmtPtr);
        node->trail = trail;
        return node;
    };

    // Optional leading USING
    std::unique_ptr<Expr> fmt;
    if (match(TokenType::KwUsing)) {
        fmt = parseExpression();
        (void)(match(TokenType::Semicolon) || match(TokenType::Comma));
    }

    // Degenerate: LPRINT ; or LPRINT , (no items)
    if (match(TokenType::Semicolon)) {
        return makePrintNode({}, PrintStmt::Terminator::Semicolon, std::move(fmt));
    }
    if (match(TokenType::Comma)) {
        return makePrintNode({}, PrintStmt::Terminator::Comma, std::move(fmt));
    }

    // Parse items and separators
    std::vector<std::unique_ptr<Expr>> items;
    std::vector<PrintStmt::Sep> seps;
    auto trail = PrintStmt::Terminator::Newline;

    auto atLineEnd = [&] {
        return check(TokenType::NewLine) || check(TokenType::Colon) || check(TokenType::EndOfFile);
    };

    auto isExprStart = [&] {
        return check(TokenType::String) || check(TokenType::Integer) || check(TokenType::Float) ||
               check(TokenType::Identifier) || check(TokenType::LParen) || check(TokenType::Plus) ||
               check(TokenType::Minus) || check(TokenType::KwNot);
    };

    for (bool done = false;!done;) {
        // Allow interspersed USING within item list
        if (check(TokenType::KwUsing)) {
            advance();
            fmt = parseExpression();
            (void)(match(TokenType::Semicolon) || match(TokenType::Comma));
            continue;
        }

        if (isExprStart()) {
            items.push_back(parseExpression());
        } else {
            // No more items to parse
            done = true;
            continue;
        }

        // Separator handling (comma/semicolon) and possible trailing terminator
        if (match(TokenType::Comma)) {
            if (atLineEnd()) {
                trail = PrintStmt::Terminator::Comma;
                done = true;
            } else {
                seps.push_back(PrintStmt::Sep::Comma);
            }
            continue;
        }

        if (match(TokenType::Semicolon)) {
            if (atLineEnd()) {
                trail = PrintStmt::Terminator::Semicolon;
                done = true;
            } else {
                seps.push_back(PrintStmt::Sep::Semicolon);
            }
            continue;
        }

        // No separator after item => end of list (newline implied)
        done = true;
    }

    auto node = make_node<PrintStmt>({l, c}, std::move(items));
    node->seps = std::move(seps);
    node->channel = channel;
    node->format = std::move(fmt);
    node->trail = trail;
    return node;
}

} // namespace gwbasic
