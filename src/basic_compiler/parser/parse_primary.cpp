// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/token/ToString.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/NumberExpr.h"
#include "basic_compiler/ast/StringExpr.h"
#include "basic_compiler/ast/CallExpr.h"
#include "basic_compiler/ast/VarExpr.h"
#include <sstream>

namespace gwbasic {

/*
 * Function: Parser::parsePrimary
 * Purpose:
 *  - Parse literals, identifiers (variables/calls), or parenthesized expressions.
 * Inputs:
 *  - none
 * Outputs:
 *  - Expr: NumberExpr, StringExpr, VarExpr, CallExpr, or nested expression
 */
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
        return make_node<NumberExpr>({l, c}, v);
    }
    if (check(TokenType::String)) {
        int l = peek().line, c = peek().col;
        std::string s = peek().lexeme;
        advance();
        return make_node<StringExpr>({l, c}, s);
    }
    if (check(TokenType::Identifier)) {
        int l = peek().line, c = peek().col;
        std::string name = peek().lexeme;
        advance();
        // Function call if immediately followed by '('
        if (match(TokenType::LParen)) {
            std::vector<std::unique_ptr<Expr>> args;
            if (!check(TokenType::RParen)) {
                do {
                    args.push_back(parseExpression());
                } while (match(TokenType::Comma));
            }
            consume(TokenType::RParen, ")");
            return make_node<CallExpr>({l, c}, name, std::move(args));
        }
        return make_node<VarExpr>({l, c}, name);
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
