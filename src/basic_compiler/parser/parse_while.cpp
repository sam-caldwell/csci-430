// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/WhileStmt.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/token/TokenType.h"
#include <memory>
#include <utility>

namespace gwbasic {

/*
 * Function: Parser::parseWhile
 * Summary:
 *  Parse WHILE condition with inline body or multiline body ending WEND.
 * Parameters:
 *  - none
 * Returns:
 *  - std::unique_ptr<Stmt>: WhileStmt node (inline or multiline)
 */
std::unique_ptr<Stmt> Parser::parseWhile() {
    auto cond = parseExpression();
    const int lineNum = peek().line;
    const int colNum = peek().col;
    auto node = make_node<WhileStmt>({lineNum, colNum}, std::move(cond));
    while (!check(TokenType::KwWend)) {
        if (check(TokenType::NewLine) || atEnd()) {
            // Multi-line WHILE: stop here; WEND will appear on later line
            return node;
        }
        if (match(TokenType::Colon)) {
            continue;
        }
        node->body.push_back(parseStatement());
        if (match(TokenType::Colon)) {
            continue;
        }
    }
    // consume WEND
    consume(TokenType::KwWend, "WEND");
    node->inlineWend = true;
    return node;
}

} // namespace gwbasic
