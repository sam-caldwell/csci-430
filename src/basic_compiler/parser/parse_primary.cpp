// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/CallExpr.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NumberExpr.h"
#include "basic_compiler/ast/StringExpr.h"
#include "basic_compiler/ast/VarExpr.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/parser/ParseError.h"
#include "basic_compiler/token/ToString.h"
#include "basic_compiler/token/TokenType.h"
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace gwbasic {

/*
 * Function: Parser::parsePrimary
 * Summary:
 *  Parse a literal, identifier (var or call), or parenthesized expression.
 * Parameters:
 *  - none
 * Returns:
 *  - std::unique_ptr<Expr>: Number, String, Var, Call, or grouped expression
 */
std::unique_ptr<Expr> Parser::parsePrimary() { // NOLINT(readability-function-size,readability-function-cognitive-complexity)
    if (check(TokenType::Integer) || check(TokenType::Float)) {
        const int lineNum = peek().line;
        const int colNum = peek().col;
        const double value = std::stod(peek().lexeme);
        advance();
        return make_node<NumberExpr>({lineNum, colNum}, value);
    }
    if (check(TokenType::String)) {
        const int lineNum = peek().line;
        const int colNum = peek().col;
        const std::string str = peek().lexeme;
        advance();
        return make_node<StringExpr>({lineNum, colNum}, str);
    }
    if (check(TokenType::Identifier)) {
        const int lineNum = peek().line;
        const int colNum = peek().col;
        const std::string name = peek().lexeme;
        advance();
        // Function call if immediately followed by '('
        if (match(TokenType::LParen)) {
            std::vector<std::unique_ptr<Expr>> args;
            if (!check(TokenType::RParen)) {
                // Parse arglist separated by commas
                args.push_back(parseExpression());
                while (match(TokenType::Comma)) {
                    args.push_back(parseExpression());
                }
            }
            consume(TokenType::RParen, ")");
            return make_node<CallExpr>({lineNum, colNum}, name, std::move(args));
        }
        return make_node<VarExpr>({lineNum, colNum}, name);
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
