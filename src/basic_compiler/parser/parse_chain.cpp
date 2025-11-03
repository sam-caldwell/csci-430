// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/ChainStmt.h"

namespace gwbasic {

std::unique_ptr<Stmt> Parser::parseChain() {
    /*
     * Function: Parser::parseChain
     * Inputs: none (assumes CHAIN already consumed)
     * Outputs: ChainStmt with optional filename, target line, and ALL flag.
     * Grammar accepted: CHAIN ["file"][, <line>][, ALL]
     */
    std::optional<std::string> file;
    std::optional<int> target;
    bool all = false;

    // Strict: filename required
    if (!check(TokenType::String)) throw ParseError("Expected filename string after CHAIN");
    file = peek().lexeme; advance();
    if (match(TokenType::Comma)) {
        if (check(TokenType::Integer)) { target = std::stoi(peek().lexeme); advance(); }
        else if (check(TokenType::KwAll)) { all = true; advance(); }
        else { throw ParseError("Expected line number or ALL after comma in CHAIN"); }
        if (match(TokenType::Comma)) {
            if (!check(TokenType::KwAll)) throw ParseError("Expected ALL after second comma in CHAIN");
            all = true; advance();
        }
    }
    return make_node<ChainStmt>({peek().line, peek().col}, std::move(file), target, all);
}

} // namespace gwbasic
