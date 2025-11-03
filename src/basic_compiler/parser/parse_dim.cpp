// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/DimStmt.h"

namespace gwbasic {

std::unique_ptr<Stmt> Parser::parseDim() {
    // DIM <Identifier> '(' <Integer> ')'
    if (!check(TokenType::Identifier)) throw ParseError("Expected array name after DIM");
    std::string name = peek().lexeme; int l = peek().line, c = peek().col; advance();
    consume(TokenType::LParen, "(");
    if (!check(TokenType::Integer)) throw ParseError("Expected array length in DIM");
    int len = std::stoi(peek().lexeme); advance();
    consume(TokenType::RParen, ")");
    return make_node<DimStmt>({l, c}, name, len);
}

} // namespace gwbasic

