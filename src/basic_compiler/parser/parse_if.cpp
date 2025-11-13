// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/GotoStmt.h"
#include "basic_compiler/ast/IfBlockStmt.h"
#include "basic_compiler/ast/IfStmt.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/parser/ParseError.h"
#include "basic_compiler/token/TokenType.h"
#include <memory>
#include <string>
#include <utility>

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
std::unique_ptr<Stmt> Parser::parseIf() { // NOLINT(readability-function-size,readability-function-cognitive-complexity)

    auto cond = parseExpression();
    const int lineNum = peek().line;
    const int colNum = peek().col;
    consume(TokenType::KwThen, "THEN");
    // Form 1: THEN <line> [ELSE <line>|ELSE GOTO <line>]
    if (check(TokenType::Integer)) {
        const int target = std::stoi(peek().lexeme);
        advance();
        // Optional ELSE arm supporting implied/explicit GOTO
        if (match(TokenType::KwElse)) {
            // Allow: ELSE <line>
            if (check(TokenType::Integer)) {
                const int elseTarget = std::stoi(peek().lexeme);
                advance();
                auto iblock = make_node<IfBlockStmt>({lineNum, colNum}, std::move(cond));
                iblock->inlineEnd = true;
                iblock->thenBody.push_back(make_node<GotoStmt>({lineNum, colNum}, target));
                iblock->elseBody.push_back(make_node<GotoStmt>({lineNum, colNum}, elseTarget));
                return iblock;
            }
            // Allow: ELSE GOTO <line>
            if (match(TokenType::KwGoto)) {
                if (!check(TokenType::Integer)) {
                    throw ParseError("Expected line number after ELSE GOTO");
                }
                const int elseTarget = std::stoi(peek().lexeme);
                advance();
                auto iblock = make_node<IfBlockStmt>({lineNum, colNum}, std::move(cond));
                iblock->inlineEnd = true;
                iblock->thenBody.push_back(make_node<GotoStmt>({lineNum, colNum}, target));
                iblock->elseBody.push_back(make_node<GotoStmt>({lineNum, colNum}, elseTarget));
                return iblock;
            }
            // Fallback: treat as inline ELSE statement list
            auto iblock = make_node<IfBlockStmt>({lineNum, colNum}, std::move(cond));
            iblock->inlineEnd = true;
            iblock->thenBody.push_back(make_node<GotoStmt>({lineNum, colNum}, target));
            if (!(check(TokenType::NewLine) || check(TokenType::EndOfFile))) {
                // Parse first ELSE statement (if any), then any colon-separated tail
                iblock->elseBody.push_back(parseStatement());
                while (match(TokenType::Colon)) {
                    iblock->elseBody.push_back(parseStatement());
                }
            }
            return iblock;
        }
        return make_node<IfStmt>({lineNum, colNum}, std::move(cond), target);
    }
    // Form 2: Inline THEN [statement-list] [ELSE [statement-list]]
    // Decide inline vs multiline: if the next token begins a statement and is not
    // NewLine/Colon/EndOfFile, parse inline lists; otherwise treat as multiline block.
    if (!(check(TokenType::NewLine) || check(TokenType::Colon) || check(TokenType::EndOfFile))) {
        auto iblock = make_node<IfBlockStmt>({lineNum, colNum}, std::move(cond));
        iblock->inlineEnd = true;
        // THEN statement-list: optionally a first statement, then colon-separated tail
        if (!(check(TokenType::KwElse) || check(TokenType::NewLine))) {
            iblock->thenBody.push_back(parseStatement());
            while (match(TokenType::Colon)) {
                iblock->thenBody.push_back(parseStatement());
            }
        }
        // Optional ELSE
        if (match(TokenType::KwElse)) {
            if (!check(TokenType::NewLine)) {
                iblock->elseBody.push_back(parseStatement());
                while (match(TokenType::Colon)) {
                    iblock->elseBody.push_back(parseStatement());
                }
            }
        }
        return iblock;
    }
    // Form 3: Multi-line IF block; body parsed by restructuring in parseProgram()
    return make_node<IfBlockStmt>({lineNum, colNum}, std::move(cond));
}

} // namespace gwbasic
