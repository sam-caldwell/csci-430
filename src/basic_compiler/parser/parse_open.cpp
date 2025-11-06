// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/OpenStmt.h"

namespace gwbasic {

/*
 * Function: Parser::parseOpen
 * Purpose:
 *  - Parse the OPEN statement: OPEN <string-expr> FOR (INPUT|OUTPUT) AS #<channel>
 * Inputs:
 *  - none (assumes 'OPEN' matched by caller)
 * Outputs:
 *  - OpenStmt: captures filename expression, file mode, and channel number
 */
std::unique_ptr<Stmt> Parser::parseOpen() {
    const int l = peek().line;
    // OPEN <string-expr> FOR (INPUT|OUTPUT) AS # <Integer>
    const int c = peek().col;
    auto fname = parseExpression();
    consume(TokenType::KwFor, "FOR");
    auto mode = FileMode::Input;
    if (match(TokenType::KwInput)) mode = FileMode::Input;
    else if (match(TokenType::KwOutput)) mode = FileMode::Output;
    else throw ParseError("Expected INPUT or OUTPUT after FOR in OPEN");
    consume(TokenType::KwAs, "AS");
    consume(TokenType::Hash, "#");
    if (!check(TokenType::Integer)) throw ParseError("Expected channel number after '#'");
    int ch = std::stoi(peek().lexeme); advance();
    return make_node<OpenStmt>({l, c}, std::move(fname), mode, ch);
}

} // namespace gwbasic
