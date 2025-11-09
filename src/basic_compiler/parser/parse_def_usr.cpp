// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/DefUsrStmt.h"
#include "basic_compiler/ast/make_node.h"
#include <cctype>
#include <optional>

namespace gwbasic {

/*
 * Function: Parser::parseDefUsr
 * Summary:
 *  Parse DEF USR[digits] = expr into a DefUsrStmt.
 * Parameters:
 *  - none
 * Returns:
 *  - std::unique_ptr<Stmt>: DefUsrStmt with optional index and address expr
 */
std::unique_ptr<Stmt> Parser::parseDefUsr() {
    // We have consumed DEF and the next token is Identifier starting with USR
    if (!check(TokenType::Identifier)) throw ParseError("Expected USR after DEF");
    std::string name = peek().lexeme; advance();
    std::string up = name; for (auto &ch : up) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
    if (up.rfind("USR", 0) != 0) throw ParseError("Expected USR after DEF");
    std::optional<int> idx;
    if (up.size() > 3) {
        bool allDigits = true; for (size_t i = 3; i < up.size(); ++i) if (!std::isdigit(static_cast<unsigned char>(up[i]))) { allDigits = false; break; }
        if (!allDigits) throw ParseError("Invalid DEF USR suffix; digits only");
        idx = std::stoi(up.substr(3));
    }
    consume(TokenType::Assign, "'='");
    auto addr = parseExpression();
    return make_node<DefUsrStmt>({0,0}, idx, std::move(addr));
}

} // namespace gwbasic
