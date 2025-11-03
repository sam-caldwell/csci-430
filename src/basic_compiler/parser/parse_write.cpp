// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/WriteStmt.h"
#include "basic_compiler/ast/InputStmt.h"

namespace gwbasic {

std::unique_ptr<Stmt> Parser::parseWrite() {
    int l = peek().line, c = peek().col;
    int channel = -1;
    if (match(TokenType::Hash)) {
        if (!check(TokenType::Integer)) throw ParseError("Expected channel number after '#'");
        channel = std::stoi(peek().lexeme); advance();
        if (match(TokenType::Comma)) {}
    }
    std::vector<std::unique_ptr<Expr>> items;
    items.push_back(parseExpression());
    while (match(TokenType::Comma)) items.push_back(parseExpression());
    return make_node<WriteStmt>({l, c}, channel, std::move(items));
}

std::unique_ptr<Stmt> Parser::parseInput() {
    // INPUT [#n,] var[,var...]  OR bare console INPUT var
    int l = peek().line, c = peek().col;
    if (match(TokenType::Hash)) {
        if (!check(TokenType::Integer)) throw ParseError("Expected channel number after '#'");
        int ch = std::stoi(peek().lexeme); advance();
        if (match(TokenType::Comma)) {}
        std::vector<std::string> vars;
        if (!check(TokenType::Identifier)) throw ParseError("Expected variable name after INPUT #n,");
        vars.push_back(peek().lexeme); advance();
        while (match(TokenType::Comma)) { if (!check(TokenType::Identifier)) throw ParseError("Expected variable"); vars.push_back(peek().lexeme); advance(); }
        return make_node<FileInputStmt>({l, c}, ch, std::move(vars));
    }
    if (!check(TokenType::Identifier)) throw ParseError("Expected variable name after INPUT");
    std::string name = peek().lexeme; advance();
    return make_node<InputStmt>({l, c}, name);
}

std::unique_ptr<Stmt> Parser::parseLineInput() {
    int l = peek().line, c = peek().col;
    int channel = -1;
    if (match(TokenType::Hash)) {
        if (!check(TokenType::Integer)) throw ParseError("Expected channel number after '#'");
        channel = std::stoi(peek().lexeme); advance();
        if (match(TokenType::Comma)) {}
    }
    if (!check(TokenType::Identifier)) throw ParseError("Expected string variable after LINE INPUT");
    std::string name = peek().lexeme; advance();
    return make_node<LineInputStmt>({l, c}, channel, name);
}

} // namespace gwbasic
