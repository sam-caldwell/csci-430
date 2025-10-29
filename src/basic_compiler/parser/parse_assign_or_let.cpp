// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"

namespace gwbasic {

std::unique_ptr<Stmt> Parser::parseAssignOrLet() {
    /*
     * Function: Parser::parseAssignOrLet
     * Inputs:
     *  - none (optionally consumes LET, then expects identifier)
     * Outputs:
     *  - AssignStmt: assignment to a variable from a parsed expression
     * Theory of operation:
     *  - Optionally consumes LET, requires an Identifier, an '=' token, then
     *    parses an expression and constructs an assignment statement.
     */
    if (match(TokenType::KwLet)) {
        // proceed to identifier
    }
    if (!check(TokenType::Identifier)) throw ParseError("Expected variable name after LET");
    std::string name = peek().lexeme;
    int l = peek().line, c = peek().col;
    advance();
    consume(TokenType::Assign, "'='");
    auto expr = parseExpression();
    auto n = std::make_unique<AssignStmt>(name, std::move(expr));
    n->pos = {l, c};
    return n;
}

} // namespace gwbasic
