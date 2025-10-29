// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/token/ToString.h"
#include <sstream>

namespace gwbasic {

std::unique_ptr<Stmt> Parser::parseStatement() {
    Token startTok = peek();
    /*
     * Function: Parser::parseStatement
     * Inputs:
     *  - none (examines current token)
     * Outputs:
     *  - std::unique_ptr<Stmt>: Parsed statement node
     * Theory of operation:
     *  - Dispatches based on the next token to the appropriate parse method
     *    (PRINT, assignment/LET, IF, FOR, GOTO, GOSUB/RETURN, INPUT, END),
     *    building the corresponding AST node or throwing on unexpected input.
     */
    if (match(TokenType::KwPrint)) { auto n = parsePrint(); n->pos = {startTok.line, startTok.col}; return n; }
    if (check(TokenType::KwLet) || check(TokenType::Identifier)) { auto n = parseAssignOrLet(); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwIf)) { auto n = parseIf(); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwFor)) { auto n = parseFor(); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwGoto)) {
        if (!check(TokenType::Integer)) throw ParseError("Expected line number after GOTO");
        int target = std::stoi(peek().lexeme);
        advance();
        auto n = std::make_unique<GotoStmt>(target);
        n->pos = {startTok.line, startTok.col};
        return n;
    }
    if (match(TokenType::KwGosub)) {
        if (!check(TokenType::Integer)) throw ParseError("Expected line number after GOSUB");
        int target = std::stoi(peek().lexeme);
        advance();
        auto n = std::make_unique<GosubStmt>(target);
        n->pos = {startTok.line, startTok.col};
        return n;
    }
    if (match(TokenType::KwReturn)) {
        auto n = std::make_unique<ReturnStmt>(); n->pos = {startTok.line, startTok.col}; return n;
    }
    if (match(TokenType::KwInput)) {
        if (!check(TokenType::Identifier)) throw ParseError("Expected variable name after INPUT");
        std::string name = peek().lexeme;
        advance();
        auto n = std::make_unique<InputStmt>(name); n->pos = {startTok.line, startTok.col}; return n;
    }
    if (match(TokenType::KwEnd)) {
        auto n = std::make_unique<EndStmt>(); n->pos = {startTok.line, startTok.col}; return n;
    }
    std::ostringstream oss;
    oss << "Unexpected token in statement: " << to_string(peek().type) << " at " << peek().line << ":" << peek().col;
    throw ParseError(oss.str());
}

} // namespace gwbasic
