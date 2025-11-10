// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/IfStmt.h"
#include "basic_compiler/ast/IfBlockStmt.h"
#include "basic_compiler/ast/GotoStmt.h"

namespace gwbasic {

/*
 * Function: Parser::parseIf
 * Summary:
 *  Parse IF ... THEN into either an IfStmt or an IfBlockStmt.
 * Parameters:
 *  - none
 * Returns:
 *  - std::unique_ptr<Stmt>: IfStmt (line target) or IfBlockStmt
 */
std::unique_ptr<Stmt> Parser::parseIf() {

    auto cond = parseExpression();
    int l = peek().line, c = peek().col;
    consume(TokenType::KwThen, "THEN");
    // Form 1: THEN <line> [ELSE <line>|ELSE GOTO <line>]
    if (check(TokenType::Integer)) {
        int target = std::stoi(peek().lexeme);
        advance();
        // Optional ELSE arm supporting implied/explicit GOTO
        if (match(TokenType::KwElse)) {
            // Allow: ELSE <line>
            if (check(TokenType::Integer)) {
                int elseTarget = std::stoi(peek().lexeme); advance();
                auto ib = make_node<IfBlockStmt>({l, c}, std::move(cond));
                ib->inlineEnd = true;
                ib->thenBody.push_back(make_node<GotoStmt>({l, c}, target));
                ib->elseBody.push_back(make_node<GotoStmt>({l, c}, elseTarget));
                return ib;
            }
            // Allow: ELSE GOTO <line>
            if (match(TokenType::KwGoto)) {
                if (!check(TokenType::Integer)) throw ParseError("Expected line number after ELSE GOTO");
                int elseTarget = std::stoi(peek().lexeme); advance();
                auto ib = make_node<IfBlockStmt>({l, c}, std::move(cond));
                ib->inlineEnd = true;
                ib->thenBody.push_back(make_node<GotoStmt>({l, c}, target));
                ib->elseBody.push_back(make_node<GotoStmt>({l, c}, elseTarget));
                return ib;
            }
            // Fallback: treat as inline ELSE statement list
            auto ib = make_node<IfBlockStmt>({l, c}, std::move(cond));
            ib->inlineEnd = true;
            ib->thenBody.push_back(make_node<GotoStmt>({l, c}, target));
            if (!(check(TokenType::NewLine) || check(TokenType::EndOfFile))) {
                // Parse first ELSE statement (if any), then any colon-separated tail
                ib->elseBody.push_back(parseStatement());
                while (match(TokenType::Colon)) {
                    ib->elseBody.push_back(parseStatement());
                }
            }
            return ib;
        }
        return make_node<IfStmt>({l, c}, std::move(cond), target);
    }
    // Form 2: Inline THEN [statement-list] [ELSE [statement-list]]
    // Decide inline vs multiline: if the next token begins a statement and is not
    // NewLine/Colon/EndOfFile, parse inline lists; otherwise treat as multiline block.
    if (!(check(TokenType::NewLine) || check(TokenType::Colon) || check(TokenType::EndOfFile))) {
        auto ib = make_node<IfBlockStmt>({l, c}, std::move(cond));
        ib->inlineEnd = true;
        // THEN statement-list: optionally a first statement, then colon-separated tail
        if (!(check(TokenType::KwElse) || check(TokenType::NewLine))) {
            ib->thenBody.push_back(parseStatement());
            while (match(TokenType::Colon)) {
                ib->thenBody.push_back(parseStatement());
            }
        }
        // Optional ELSE
        if (match(TokenType::KwElse)) {
            if (!check(TokenType::NewLine)) {
                ib->elseBody.push_back(parseStatement());
                while (match(TokenType::Colon)) {
                    ib->elseBody.push_back(parseStatement());
                }
            }
        }
        return ib;
    }
    // Form 3: Multi-line IF block; body parsed by restructuring in parseProgram()
    return make_node<IfBlockStmt>({l, c}, std::move(cond));
}

} // namespace gwbasic
