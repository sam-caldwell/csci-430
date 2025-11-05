// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/AssignStmt.h"
#include "basic_compiler/ast/ArrayAssignStmt.h"
#include "basic_compiler/ast/MidAssignStmt.h"

namespace gwbasic {

/*
 * Function: Parser::parseAssignOrLet
 * Purpose:
 *  - Parse assignment statements with optional leading LET keyword.
 * Inputs:
 *  - none (assumes current token is LET or Identifier)
 * Outputs:
 *  - AssignStmt or ArrayAssignStmt depending on presence of '(index)'
 */
std::unique_ptr<Stmt> Parser::parseAssignOrLet() {
    if (match(TokenType::KwLet)) {
        // Special-case: LET MID$(...) = expr$
        if (check(TokenType::Identifier)) {
            std::string up = peek().lexeme; for (auto &ch : up) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
            if (up == "MID$" && peekNext().type == TokenType::LParen) {
                const int l = peek().line, c = peek().col;
                advance(); // consume MID$
                consume(TokenType::LParen, "(");
                if (!check(TokenType::Identifier)) throw ParseError("Expected string variable name in MID$ assignment");
                std::string name = peek().lexeme; advance();
                std::unique_ptr<Expr> idx;
                if (match(TokenType::LParen)) { idx = parseExpression(); consume(TokenType::RParen, ")"); }
                consume(TokenType::Comma, ",");
                auto start = parseExpression();
                std::unique_ptr<Expr> len;
                if (match(TokenType::Comma)) len = parseExpression();
                consume(TokenType::RParen, ")");
                consume(TokenType::Assign, "'='");
                auto value = parseExpression();
                return make_node<MidAssignStmt>({l, c}, name, std::move(idx), std::move(start), std::move(len), std::move(value));
            }
        }
        // proceed to identifier (normal assignment)
    }
    if (!check(TokenType::Identifier)) throw ParseError("Expected variable name after LET");
    std::string name = peek().lexeme;
    const int l = peek().line;
    int c = peek().col;
    advance();
    // Array element assignment A(expr) = ...
    if (match(TokenType::LParen)) {
        auto idx = parseExpression();
        consume(TokenType::RParen, ")");
        consume(TokenType::Assign, "'='");
        auto expr = parseExpression();
        return make_node<ArrayAssignStmt>({l, c}, name, std::move(idx), std::move(expr));
    }
    consume(TokenType::Assign, "'='");
    auto expr = parseExpression();
    return make_node<AssignStmt>({l, c}, name, std::move(expr));
}

} // namespace gwbasic
