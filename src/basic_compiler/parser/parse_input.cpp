// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/FileInputStmt.h"
#include "basic_compiler/ast/InputStmt.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/parser/ParseError.h"
#include "basic_compiler/token/TokenType.h"
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace gwbasic {

/*
 * Function: Parser::parseInput
 * Summary:
 *  Parse INPUT [#n,] var-list or console INPUT with prompt.
 * Parameters:
 *  - none
 * Returns:
 *  - std::unique_ptr<Stmt>: FileInputStmt (with channel) or InputStmt
 */
std::unique_ptr<Stmt> Parser::parseInput() { // NOLINT(readability-function-size,readability-function-cognitive-complexity)
    const int lineNum = peek().line;
    const int colNum = peek().col;
    if (match(TokenType::Hash)) {
        if (!check(TokenType::Integer)) {
            throw ParseError("Expected channel number after '#'");
        }
        const int channel = std::stoi(peek().lexeme);
        advance();
        if (match(TokenType::Comma)) {
            // ok
        }
        std::vector<std::string> vars;
        if (!check(TokenType::Identifier)) {
            throw ParseError("Expected variable name after INPUT #n,");
        }
        vars.push_back(peek().lexeme);
        advance();
        while (match(TokenType::Comma)) {
            if (!check(TokenType::Identifier)) {
                throw ParseError("Expected variable");
            }
            vars.push_back(peek().lexeme);
            advance();
        }
        return make_node<FileInputStmt>({lineNum, colNum}, channel, std::move(vars));
    }
    // Console INPUT: optional prompt, then var list
    std::optional<std::string> promptLit;
    std::optional<std::string> promptVar;
    // Case 1: INPUT ; P$, var...
    if (match(TokenType::Semicolon)) {
        if (!check(TokenType::Identifier)) {
            throw ParseError("Expected string variable after ';' in INPUT");
        }
        promptVar = peek().lexeme;
        advance();
        // Expect a comma before variables
        if (!match(TokenType::Comma)) {
            throw ParseError("Expected ',' after prompt variable in INPUT");
        }
    } else if (check(TokenType::String)) {
        // Case 2: INPUT "literal"; var...
        promptLit = peek().lexeme;
        advance();
        consume(TokenType::Semicolon, "';' after prompt string in INPUT");
    }
    // Parse variable list (at least one)
    std::vector<std::string> vars;
    if (!check(TokenType::Identifier)) {
        throw ParseError("Expected variable name after INPUT");
    }
    vars.push_back(peek().lexeme);
    advance();
    while (match(TokenType::Comma)) {
        if (!check(TokenType::Identifier)) {
            throw ParseError("Expected variable name after ',' in INPUT");
        }
        vars.push_back(peek().lexeme);
        advance();
    }
    auto node = make_node<InputStmt>({lineNum, colNum}, std::move(vars));
    node->promptLiteral = promptLit;
    node->promptVar = promptVar;
    return node;
}

} // namespace gwbasic
