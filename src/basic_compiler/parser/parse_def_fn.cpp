// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/parser/ParseError.h"
#include "basic_compiler/token/ToString.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/DefFnStmt.h"
#include <sstream>
#include <cctype>

namespace gwbasic {

/*
 * Function: Parser::parseDefFn
 * Summary:
 *  Parse DEF FNname(param) = expression into a DefFnStmt.
 * Parameters:
 *  - none
 * Returns:
 *  - std::unique_ptr<Stmt>: DefFnStmt with name, parameter, and body
 */
std::unique_ptr<Stmt> Parser::parseDefFn() {
    if (!check(TokenType::Identifier)) throw ParseError("Expected FNname after DEF");
    std::string fn = peek().lexeme;
    advance();
    // Require prefix FN (case-insensitive)
    {
        std::string up = fn; for (auto &ch: up) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
        if (up.size() < 3 || up.rfind("FN", 0) != 0) {
            std::ostringstream m; m << "Expected FNname after DEF, got '" << fn << "'"; throw ParseError(m.str());
        }
    }
    consume(TokenType::LParen, "'('");
    if (!check(TokenType::Identifier)) throw ParseError("Expected parameter name in DEF FN");
    std::string param = peek().lexeme;
    advance();
    consume(TokenType::RParen, "')'");
    consume(TokenType::Assign, "'='");
    auto body = parseExpression();
    return make_node<DefFnStmt>({0,0}, fn, param, std::move(body));
}

} // namespace gwbasic
