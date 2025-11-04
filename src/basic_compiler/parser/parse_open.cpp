// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/OpenStmt.h"
#include "basic_compiler/ast/CloseStmt.h"

namespace gwbasic {

std::unique_ptr<Stmt> Parser::parseOpen() {
    // OPEN <string-expr> FOR (INPUT|OUTPUT) AS # <Integer>
    int l = peek().line, c = peek().col;
    auto fname = parseExpression();
    consume(TokenType::KwFor, "FOR");
    FileMode mode = FileMode::Input;
    if (match(TokenType::KwInput)) mode = FileMode::Input;
    else if (match(TokenType::KwOutput)) mode = FileMode::Output;
    else throw ParseError("Expected INPUT or OUTPUT after FOR in OPEN");
    consume(TokenType::KwAs, "AS");
    consume(TokenType::Hash, "#");
    if (!check(TokenType::Integer)) throw ParseError("Expected channel number after '#'");
    int ch = std::stoi(peek().lexeme); advance();
    return make_node<OpenStmt>({l, c}, std::move(fname), mode, ch);
}

std::unique_ptr<Stmt> Parser::parseClose() {
    // CLOSE # <Integer>
    int l = peek().line, c = peek().col;
    consume(TokenType::Hash, "#");
    if (!check(TokenType::Integer)) throw ParseError("Expected channel number after '#'");
    int ch = std::stoi(peek().lexeme); advance();
    return make_node<CloseStmt>({l, c}, ch);
}

} // namespace gwbasic
