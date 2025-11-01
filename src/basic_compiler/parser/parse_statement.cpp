// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/token/ToString.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/GotoStmt.h"
#include "basic_compiler/ast/GosubStmt.h"
#include "basic_compiler/ast/ReturnStmt.h"
#include "basic_compiler/ast/InputStmt.h"
#include "basic_compiler/ast/RandomizeStmt.h"
#include "basic_compiler/ast/EndStmt.h"
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
        return make_node<GotoStmt>({startTok.line, startTok.col}, target);
    }
    if (match(TokenType::KwGosub)) {
        if (!check(TokenType::Integer)) throw ParseError("Expected line number after GOSUB");
        int target = std::stoi(peek().lexeme);
        advance();
        return make_node<GosubStmt>({startTok.line, startTok.col}, target);
    }
    if (match(TokenType::KwReturn)) { return make_node<ReturnStmt>({startTok.line, startTok.col}); }
    if (match(TokenType::KwInput)) {
        if (!check(TokenType::Identifier)) throw ParseError("Expected variable name after INPUT");
        std::string name = peek().lexeme;
        advance();
        return make_node<InputStmt>({startTok.line, startTok.col}, name);
    }
    if (match(TokenType::KwRandomize)) {
        std::unique_ptr<Expr> seed;
        // Optional expression if the next token can start an expression
        // Accept: number, string? (we'll reject type in semantics), identifier, '('
        if (!(check(TokenType::NewLine) || check(TokenType::Colon) || check(TokenType::EndOfFile))) {
            // If the next token is THEN (from IF context), or other stmt starter, we should not parse; but within parseStatement
            // we are at a statement boundary, so proceed
            seed = parseExpression();
        }
        return make_node<RandomizeStmt>({startTok.line, startTok.col}, std::move(seed));
    }
    if (match(TokenType::KwEnd)) { return make_node<EndStmt>({startTok.line, startTok.col}); }
    std::ostringstream oss;
    oss << "Unexpected token in statement: " << to_string(peek().type) << " at " << peek().line << ":" << peek().col;
    throw ParseError(oss.str());
}

} // namespace gwbasic
