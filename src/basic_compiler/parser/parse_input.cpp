// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/FileInputStmt.h"
#include "basic_compiler/ast/InputStmt.h"

namespace gwbasic {

/*
 * Function: Parser::parseInput
 * Purpose:
 *  - Parse INPUT [#n,] var[,var...] or bare console INPUT [; P$ ,] ["text";] var[,var...]
 * Inputs:
 *  - none (assumes 'INPUT' matched by caller)
 * Outputs:
 *  - FileInputStmt when a channel is specified; otherwise InputStmt
 */
std::unique_ptr<Stmt> Parser::parseInput() {
    const int l = peek().line;
    const int c = peek().col;
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
    // Console INPUT: optional prompt, then var list
    std::optional<std::string> promptLit;
    std::optional<std::string> promptVar;
    // Case 1: INPUT ; P$, var...
    if (match(TokenType::Semicolon)) {
        if (!check(TokenType::Identifier)) throw ParseError("Expected string variable after ';' in INPUT");
        promptVar = peek().lexeme; advance();
        // Expect a comma before variables
        if (!match(TokenType::Comma)) throw ParseError("Expected ',' after prompt variable in INPUT");
    } else if (check(TokenType::String)) {
        // Case 2: INPUT "literal"; var...
        promptLit = peek().lexeme; advance();
        consume(TokenType::Semicolon, "';' after prompt string in INPUT");
    }
    // Parse variable list (at least one)
    std::vector<std::string> vars;
    if (!check(TokenType::Identifier)) throw ParseError("Expected variable name after INPUT");
    vars.push_back(peek().lexeme); advance();
    while (match(TokenType::Comma)) {
        if (!check(TokenType::Identifier)) throw ParseError("Expected variable name after ',' in INPUT");
        vars.push_back(peek().lexeme); advance();
    }
    auto node = make_node<InputStmt>({l, c}, std::move(vars));
    node->promptLiteral = promptLit;
    node->promptVar = promptVar;
    return node;
}

} // namespace gwbasic
