// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/DefFnStmt.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/parser/ParseError.h"
#include "basic_compiler/token/TokenType.h"
#include <cctype>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

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
    if (!check(TokenType::Identifier)) {
        throw ParseError("Expected FNname after DEF");
    }
    const std::string funcName = peek().lexeme;
    advance();
    // Require prefix FN (case-insensitive)
    {
        std::string upper = funcName;
        for (auto &chr : upper) {
            chr = static_cast<char>(std::toupper(static_cast<unsigned char>(chr)));
        }
        if (upper.size() < 3 || upper.rfind("FN", 0) != 0) {
            std::ostringstream msg;
            msg << "Expected FNname after DEF, got '" << funcName << "'";
            throw ParseError(msg.str());
        }
    }
    consume(TokenType::LParen, "'('");
    if (!check(TokenType::Identifier)) {
        throw ParseError("Expected parameter name in DEF FN");
    }
    const std::string param = peek().lexeme;
    advance();
    consume(TokenType::RParen, "')'");
    consume(TokenType::Assign, "'='");
    auto body = parseExpression();
    return make_node<DefFnStmt>({0,0}, funcName, param, std::move(body));
}

} // namespace gwbasic
