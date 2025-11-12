// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/DimStmt.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/parser/ParseError.h"
#include "basic_compiler/token/TokenType.h"
#include <memory>
#include <string>
#include <utility>
#include <vector>

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
    if (!check(TokenType::Identifier)) {
        throw ParseError("Expected array name after DIM");
    }
    const std::string name = peek().lexeme;
    const int lineNum = peek().line;
    const int colNum = peek().col;
    advance();
    consume(TokenType::LParen, "(");
    std::vector<int> bounds;
    if (!check(TokenType::Integer)) {
        throw ParseError("Expected array bound in DIM");
    }
    while (true) {
        if (!check(TokenType::Integer)) {
            throw ParseError("Expected integer bound in DIM");
        }
        const int boundVal = std::stoi(peek().lexeme);
        advance();
        bounds.push_back(boundVal);
        if (!match(TokenType::Comma)) {
            break;
        }
    }
    consume(TokenType::RParen, ")");
    return make_node<DimStmt>({lineNum, colNum}, name, std::move(bounds));
}

} // namespace gwbasic
