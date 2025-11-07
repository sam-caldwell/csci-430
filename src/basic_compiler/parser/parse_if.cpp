// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/IfStmt.h"
#include "basic_compiler/ast/IfBlockStmt.h"

namespace gwbasic {

/*
 * Function: Parser::parseIf
 * Inputs:
 *  - none (assumes IF already consumed)
 * Outputs:
 *  - IfStmt (single-line THEN <line>) or IfBlockStmt (multi-line)
 * Theory of operation:
 *  - Parses comparison expression and THEN.
 *    If the next token is a line number, produce IfStmt.
 *    Otherwise, treat it as a multi-line IF block to be folded later.
 */
std::unique_ptr<Stmt> Parser::parseIf() {

    auto cond = parseExpression();
    int l = peek().line, c = peek().col;
    consume(TokenType::KwThen, "THEN");
    // Form 1: THEN <line>
    if (check(TokenType::Integer)) {
        int target = std::stoi(peek().lexeme);
        advance();
        return make_node<IfStmt>({l, c}, std::move(cond), target);
    }
    // Form 2: Inline THEN [statement-list] [ELSE [statement-list]]
    // Decide inline vs multiline: if the next token begins a statement and is not
    // NewLine/Colon/EndOfFile, parse inline lists; otherwise treat as multiline block.
    if (!(check(TokenType::NewLine) || check(TokenType::Colon) || check(TokenType::EndOfFile))) {
        auto ib = make_node<IfBlockStmt>({l, c}, std::move(cond));
        ib->inlineEnd = true;
        // THEN statement-list: parse statements until ELSE or end-of-line
        while (!atEnd()) {
            if (check(TokenType::KwElse)) break; // do not consume here
            // Stop if end of line
            if (check(TokenType::NewLine)) break;
            // Parse a statement into thenBody
            auto st = parseStatement();
            ib->thenBody.push_back(std::move(st));
            // THEN list may be colon-separated
            if (match(TokenType::Colon)) {
                // If next token is ELSE immediately after colon, handle in next loop
                continue;
            }
            // Otherwise stop at end-of-line
            if (check(TokenType::NewLine) || check(TokenType::EndOfFile)) break;
            // If ELSE appears next, loop will break
        }
        // Optional ELSE
        if (match(TokenType::KwElse)) {
            while (!atEnd()) {
                if (check(TokenType::NewLine)) break;
                auto st = parseStatement();
                ib->elseBody.push_back(std::move(st));
                if (match(TokenType::Colon)) continue;
                if (check(TokenType::NewLine) || check(TokenType::EndOfFile)) break;
            }
        }
        return ib;
    }
    // Form 3: Multi-line IF block; body parsed by restructuring in parseProgram()
    return make_node<IfBlockStmt>({l, c}, std::move(cond));
}

} // namespace gwbasic
