// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/FileInputStmt.h"
#include "basic_compiler/ast/InputStmt.h"

namespace gwbasic {

/*
 * Function: Parser::parseInput
 * Purpose:
 *  - Parse INPUT [#n,] var[,var...] or bare console INPUT var
 * Inputs:
 *  - none (assumes 'INPUT' was matched by caller)
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
    if (!check(TokenType::Identifier)) throw ParseError("Expected variable name after INPUT");
    std::string name = peek().lexeme; advance();
    return make_node<InputStmt>({l, c}, name);
}

} // namespace gwbasic

