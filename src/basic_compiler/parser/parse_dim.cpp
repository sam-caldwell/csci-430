// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/DimStmt.h"

namespace gwbasic {

/*
 * Function: Parser::parseDim
 * Purpose:
 *  - Parse DIM name(bound[,bound...]) where bounds are integers (upper bounds inclusive)
 * Inputs:
 *  - none (assumes 'DIM' matched by caller)
 * Outputs:
 *  - DimStmt: array name and length
 */
std::unique_ptr<Stmt> Parser::parseDim() {
    // DIM <Identifier> '(' <Integer> [',' <Integer>]* ')'
    if (!check(TokenType::Identifier)) throw ParseError("Expected array name after DIM");
    std::string name = peek().lexeme;
    const int l = peek().line;
    const int c = peek().col; advance();
    consume(TokenType::LParen, "(");
    std::vector<int> bounds;
    if (!check(TokenType::Integer)) throw ParseError("Expected array bound in DIM");
    do {
        if (!check(TokenType::Integer)) throw ParseError("Expected integer bound in DIM");
        int b = std::stoi(peek().lexeme); advance();
        bounds.push_back(b);
    } while (match(TokenType::Comma));
    consume(TokenType::RParen, ")");
    return make_node<DimStmt>({l, c}, name, std::move(bounds));
}

} // namespace gwbasic
