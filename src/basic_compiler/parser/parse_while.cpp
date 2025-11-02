// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/WhileStmt.h"

namespace gwbasic {

std::unique_ptr<Stmt> Parser::parseWhile() {
    /*
     * Single-line WHILE ... WEND
     *  - Parse comparison expression as condition
     *  - Collect statements until WEND on the same line
     */
    auto cond = parseComparison();
    int l = peek().line, c = peek().col;
    auto node = make_node<WhileStmt>({l, c}, std::move(cond));
    while (!check(TokenType::KwWend)) {
        if (check(TokenType::NewLine) || atEnd()) {
            // Multi-line WHILE: stop here; WEND will appear on later line
            return node;
        }
        if (match(TokenType::Colon)) continue;
        node->body.push_back(parseStatement());
        if (match(TokenType::Colon)) continue;
    }
    // consume WEND
    consume(TokenType::KwWend, "WEND");
    node->inlineWend = true;
    return node;
}

} // namespace gwbasic
