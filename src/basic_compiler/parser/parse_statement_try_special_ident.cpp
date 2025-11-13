// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "../../../include/basic_compiler/parser/Parser.h"
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
 * Function: Parser::tryParseSpecialIdentifierStatement
 * Summary:
 *  Handle identifier-led statements (e.g., SCREEN, CIRCLE, MID$ assignment).
 * Parameters:
 *  - startTok: Token at which the statement begins for position assignment
 * Returns:
 *  - std::unique_ptr<Stmt>: Parsed node or nullptr if not matched
 */
std::unique_ptr<Stmt> Parser::tryParseSpecialIdentifierStatement(const Token& startTok) { // NOLINT(readability-function-size,readability-function-cognitive-complexity)
    if (!check(TokenType::Identifier)) {
        return nullptr;
    }
    std::string upper = peek().lexeme;
    for (auto &chr : upper) {
        chr = static_cast<char>(std::toupper(static_cast<unsigned char>(chr)));
    }
    if (upper == "SCREEN") {
        if (peekNext().type != TokenType::LParen) {
            advance();
            auto node = parseScreen();
            node->pos = {startTok.line, startTok.col};
            return node;
        }
        return nullptr; // function form: not a statement
    }
    if (upper == "CIRCLE") {
        advance();
        auto node = parseCircle();
        node->pos = {startTok.line, startTok.col};
        return node;
    }
    if (upper == "MID$") {
        // Bare MID$ assignment at statement start: MID$(s$, start[, len]) = expr$
        // Only parse here if followed by '(' to avoid colliding with variable names
        if (peekNext().type != TokenType::LParen) {
            return nullptr;
        }
        advance(); // consume MID$
        consume(TokenType::LParen, "(");
        if (!check(TokenType::Identifier)) {
            throw ParseError("Expected string variable name in MID$ assignment");
        }
        const std::string name = peek().lexeme;
        advance();
        std::vector<std::unique_ptr<Expr>> indices;
        if (match(TokenType::LParen)) {
            // Parse one or more indices separated by commas
            indices.push_back(parseExpression());
            while (match(TokenType::Comma)) {
                indices.push_back(parseExpression());
            }
            consume(TokenType::RParen, ")");
        }
        consume(TokenType::Comma, ",");
        auto start = parseExpression();
        std::unique_ptr<Expr> len;
        if (match(TokenType::Comma)) {
            len = parseExpression();
        }
        consume(TokenType::RParen, ")");
        consume(TokenType::Assign, "'='");
        auto value = parseExpression();
        auto node = make_node<MidAssignStmt>({startTok.line, startTok.col}, name, std::move(indices), std::move(start), std::move(len), std::move(value));
        return node;
    }
    return nullptr;
}

} // namespace gwbasic
