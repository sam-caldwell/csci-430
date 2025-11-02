// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/IfStmt.h"
#include "basic_compiler/ast/IfBlockStmt.h"

namespace gwbasic {

std::unique_ptr<Stmt> Parser::parseIf() {
    /*
     * Function: Parser::parseIf
     * Inputs:
     *  - none (assumes IF already consumed)
     * Outputs:
     *  - IfStmt (single-line THEN <line>) or IfBlockStmt (multi-line)
     * Theory of operation:
     *  - Parses comparison expression and THEN.
     *    If next token is a line number, produce IfStmt.
     *    Otherwise, treat as multi-line IF block to be folded later.
     */
    auto cond = parseComparison();
    int l = peek().line, c = peek().col;
    consume(TokenType::KwThen, "THEN");
    if (check(TokenType::Integer)) {
        int target = std::stoi(peek().lexeme);
        advance();
        return make_node<IfStmt>({l, c}, std::move(cond), target);
    }
    // Multi-line IF block (expect newline or ':' followed by body on same line)
    return make_node<IfBlockStmt>({l, c}, std::move(cond));
}

} // namespace gwbasic
