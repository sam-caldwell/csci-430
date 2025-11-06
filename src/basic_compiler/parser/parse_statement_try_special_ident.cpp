// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/MidAssignStmt.h"
#include <cctype>

namespace gwbasic {

/*
 * Function: Parser::tryParseSpecialIdentifierStatement
 * Inputs:
 *  - startTok: Token at which the statement begins (for pos assignment)
 * Outputs:
 *  - std::unique_ptr<Stmt> when a special identifier-led statement is parsed;
 *    otherwise nullptr and no tokens are consumed.
 * Theory of operation:
 *  - Some statements are introduced by identifiers rather than keywords
 *    (e.g., SCREEN, CIRCLE). When these appear and are not function calls,
 *    parse them before falling back to generic identifier assignment parsing.
 */
std::unique_ptr<Stmt> Parser::tryParseSpecialIdentifierStatement(const Token& startTok) {
    if (!check(TokenType::Identifier)) return nullptr;
    std::string up = peek().lexeme;
    for (auto &ch: up) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
    if (up == "SCREEN") {
        if (peekNext().type != TokenType::LParen) {
            advance();
            auto n = parseScreen();
            n->pos = {startTok.line, startTok.col};
            return n;
        }
        return nullptr; // function form: not a statement
    }
    if (up == "CIRCLE") {
        advance();
        auto n = parseCircle();
        n->pos = {startTok.line, startTok.col};
        return n;
    }
    if (up == "MID$") {
        // Bare MID$ assignment at statement start: MID$(s$, start[, len]) = expr$
        // Only parse here if followed by '(' to avoid colliding with variable names
        if (peekNext().type != TokenType::LParen) return nullptr;
        advance(); // consume MID$
        consume(TokenType::LParen, "(");
        if (!check(TokenType::Identifier)) throw ParseError("Expected string variable name in MID$ assignment");
        std::string name = peek().lexeme; advance();
        std::vector<std::unique_ptr<Expr>> indices;
        if (match(TokenType::LParen)) {
            // Parse one or more indices separated by commas
            indices.push_back(parseExpression());
            while (match(TokenType::Comma)) indices.push_back(parseExpression());
            consume(TokenType::RParen, ")");
        }
        consume(TokenType::Comma, ",");
        auto start = parseExpression();
        std::unique_ptr<Expr> len;
        if (match(TokenType::Comma)) len = parseExpression();
        consume(TokenType::RParen, ")");
        consume(TokenType::Assign, "'='");
        auto value = parseExpression();
        auto n = make_node<MidAssignStmt>({startTok.line, startTok.col}, name, std::move(indices), std::move(start), std::move(len), std::move(value));
        return n;
    }
    return nullptr;
}

} // namespace gwbasic
