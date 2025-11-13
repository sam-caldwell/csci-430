// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/ArrayAssignStmt.h"
#include "basic_compiler/ast/AssignStmt.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/MidAssignStmt.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/parser/ParseError.h"
#include "basic_compiler/token/Token.h"
#include "basic_compiler/token/TokenType.h"
#include <cctype>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace gwbasic {

/*
 * Function: Parser::parseAssignOrLet
 * Summary:
 *  Parse LET/assignment for scalars, arrays, and MID$ slice assignments.
 * Parameters:
 *  - none
 * Returns:
 *  - std::unique_ptr<Stmt>: AssignStmt, ArrayAssignStmt, or MidAssignStmt
 */
std::unique_ptr<Stmt> Parser::parseAssignOrLet() { // NOLINT(readability-function-size,readability-function-cognitive-complexity)
    if (match(TokenType::KwLet)) {
        if (check(TokenType::Identifier)) {
            if (auto mid = parseMidAssignInLet(peek())) {
                return mid;
            }
        }
        // proceed to identifier (normal assignment)
    }
    if (!check(TokenType::Identifier)) {
        throw ParseError("Expected variable name after LET");
    }
    const std::string name = peek().lexeme;
    const int lineNum = peek().line;
    const int colNum = peek().col;
    advance();
    // Array element assignment A(expr) = ...
    if (match(TokenType::LParen)) {
        return parseArrayAssignTail(name, lineNum, colNum);
    }
    consume(TokenType::Assign, "'='");
    auto expr = parseExpression();
    return make_node<AssignStmt>({lineNum, colNum}, name, std::move(expr));
}

/*
 * Function: Parser::parseMidAssignInLet
 * Summary:
 *  Handle LET MID$(var[,indexes]) = <string-expr> assignments.
 * Parameters:
 *  - startTok: Token where the statement begins (for position tagging).
 * Returns:
 *  - std::unique_ptr<Stmt>: MidAssignStmt if matched; nullptr otherwise.
 */
std::unique_ptr<Stmt> Parser::parseMidAssignInLet(const Token& startTok) { // NOLINT(readability-function-size,readability-function-cognitive-complexity)
    std::string upperName = peek().lexeme;
    for (auto &chChar : upperName) {
        chChar = static_cast<char>(std::toupper(static_cast<unsigned char>(chChar)));
    }
    if (upperName != "MID$" || peekNext().type != TokenType::LParen) {
        return nullptr;
    }
    const int lineNum = startTok.line;
    const int colNum = startTok.col;
    advance(); // consume MID$
    consume(TokenType::LParen, "(");
    if (!check(TokenType::Identifier)) {
        throw ParseError("Expected string variable name in MID$ assignment");
    }
    const std::string name = peek().lexeme;
    advance();
    std::vector<std::unique_ptr<Expr>> indices;
    if (match(TokenType::LParen)) {
        while (!check(TokenType::RParen)) {
            indices.push_back(parseExpression());
            if (!match(TokenType::Comma)) {
                break;
            }
        }
        consume(TokenType::RParen, ")");
    }
    consume(TokenType::Comma, ",");
    auto start = parseExpression();
    std::unique_ptr<Expr> sliceLenExpr;
    if (match(TokenType::Comma)) {
        sliceLenExpr = parseExpression();
    }
    consume(TokenType::RParen, ")");
    consume(TokenType::Assign, "'='");
    auto value = parseExpression();
    return make_node<MidAssignStmt>({lineNum, colNum}, name, std::move(indices), std::move(start), std::move(sliceLenExpr), std::move(value));
}

/*
 * Function: Parser::parseArrayAssignTail
 * Summary:
 *  Parse the tail of an array assignment after encountering name '('.
 * Parameters:
 *  - name: Array variable identifier.
 *  - lineNum: Source line for position tagging.
 *  - colNum: Source column for position tagging.
 * Returns:
 *  - std::unique_ptr<Stmt>: ArrayAssignStmt with indices and value.
 */
std::unique_ptr<Stmt> Parser::parseArrayAssignTail(const std::string& name, int lineNum, int colNum) {
    std::vector<std::unique_ptr<Expr>> indices;
    while (!check(TokenType::RParen)) {
        indices.push_back(parseExpression());
        if (!match(TokenType::Comma)) {
            break;
        }
    }
    consume(TokenType::RParen, ")");
    consume(TokenType::Assign, "'='");
    auto expr = parseExpression();
    return make_node<ArrayAssignStmt>({lineNum, colNum}, name, std::move(indices), std::move(expr));
}

} // namespace gwbasic
