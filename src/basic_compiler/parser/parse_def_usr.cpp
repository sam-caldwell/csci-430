// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/DefUsrStmt.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/parser/ParseError.h"
#include "basic_compiler/token/TokenType.h"
#include <cctype>
#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <utility>

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
std::unique_ptr<Stmt> Parser::parseDefUsr() { // NOLINT(readability-function-size,readability-function-cognitive-complexity)
    // We have consumed DEF and the next token is Identifier starting with USR
    if (!check(TokenType::Identifier)) {
        throw ParseError("Expected USR after DEF");
    }
    const std::string usrName = peek().lexeme;
    advance();
    std::string upper = usrName;
    for (auto &chr : upper) {
        chr = static_cast<char>(
            std::toupper(static_cast<unsigned char>(chr))
        );
    }
    if (upper.rfind("USR", 0) != 0) {
        throw ParseError("Expected USR after DEF");
    }
    std::optional<int> idx;
    if (upper.size() > 3) {
        bool allDigits = true;
        for (size_t pos = 3; pos < upper.size(); ++pos) {
            if (std::isdigit(static_cast<unsigned char>(upper[pos])) == 0) {
                allDigits = false;
                break;
            }
        }
        if (!allDigits) {
            throw ParseError("Invalid DEF USR suffix; digits only");
        }
        idx = std::stoi(upper.substr(3));
    }
    consume(TokenType::Assign, "'='");
    auto addr = parseExpression();
    return make_node<DefUsrStmt>({0,0}, idx, std::move(addr));
}

} // namespace gwbasic
