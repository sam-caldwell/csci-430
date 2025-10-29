// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/token/ToString.h"
#include <sstream>

namespace gwbasic {

std::unique_ptr<Expr> Parser::parsePrimary() {
    /*
     * Function: Parser::parsePrimary
     * Inputs:
     *  - none
     * Outputs:
     *  - Expr: a number, string, variable, or parenthesized expression
     * Theory of operation:
     *  - Recognizes literal tokens, identifiers, or '(' expression ')';
     *    throws ParseError for any unexpected token.
     */
    if (check(TokenType::Integer) || check(TokenType::Float)) {
        int l = peek().line, c = peek().col;
        double v = std::stod(peek().lexeme);
        advance();
        auto n = std::make_unique<NumberExpr>(v);
        n->pos = {l, c};
        return n;
    }
    if (check(TokenType::String)) {
        int l = peek().line, c = peek().col;
        std::string s = peek().lexeme;
        advance();
        auto n = std::make_unique<StringExpr>(s);
        n->pos = {l, c};
        return n;
    }
    if (check(TokenType::Identifier)) {
        int l = peek().line, c = peek().col;
        std::string n = peek().lexeme;
        advance();
        auto v = std::make_unique<VarExpr>(n);
        v->pos = {l, c};
        return v;
    }
    if (match(TokenType::LParen)) {
        auto expr = parseExpression();
        consume(TokenType::RParen, ")");
        return expr;
    }
    std::ostringstream oss;
    oss << "Unexpected token in expression: " << to_string(peek().type) << " at " << peek().line << ":" << peek().col;
    throw ParseError(oss.str());
}

} // namespace gwbasic
