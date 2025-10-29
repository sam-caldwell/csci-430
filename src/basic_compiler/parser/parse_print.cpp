// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"

namespace gwbasic {

std::unique_ptr<Stmt> Parser::parsePrint() {
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
    if (check(TokenType::String)) {
        std::string s = peek().lexeme;
        int l = peek().line, c = peek().col;
        advance();
        auto n = std::make_unique<PrintStmt>(std::make_unique<StringExpr>(s));
        n->pos = {l, c};
        return n;
    }
    auto expr = parseExpression();
    // Capture position before moving from the unique_ptr to avoid use-after-move
    const int eline = expr->pos.line;
    const int ecol = expr->pos.col;
    auto n = std::make_unique<PrintStmt>(std::move(expr));
    n->pos = {eline, ecol};
    return n;
}

} // namespace gwbasic
