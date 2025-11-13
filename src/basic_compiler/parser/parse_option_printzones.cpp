// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/OptionPrintZonesStmt.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/parser/ParseError.h"
#include "basic_compiler/token/TokenType.h"
#include <cctype>
#include <memory>
#include <string>

namespace gwbasic {

/*
 * Function: Parser::parseOptionPrintZones
 * Summary:
 *  Parse OPTION PRINTZONES ON|OFF into an OptionPrintZonesStmt.
 * Parameters:
 *  - none
 * Returns:
 *  - std::unique_ptr<Stmt>: OptionPrintZonesStmt with ON/OFF state
 */
std::unique_ptr<Stmt> Parser::parseOptionPrintZones() {
    // Expect identifier PRINTZONES
    if (peek().type != TokenType::Identifier) {
        throw ParseError("Expected PRINTZONES after OPTION");
    }
    std::string upper = peek().lexeme;
    for (auto &chr : upper) {
        chr = static_cast<char>(std::toupper(static_cast<unsigned char>(chr)));
    }
    advance();
    if (upper != "PRINTZONES") {
        throw ParseError("Expected PRINTZONES after OPTION");
    }
    // Expect ON or OFF
    bool enable = false;
    if (match(TokenType::KwOn)) {
        enable = true;
    } else {
        // OFF may be lexed as Identifier
        if (peek().type != TokenType::Identifier) {
            throw ParseError("Expected ON or OFF after PRINTZONES");
        }
        std::string upper2 = peek().lexeme;
        for (auto &chr : upper2) {
            chr = static_cast<char>(std::toupper(static_cast<unsigned char>(chr)));
        }
        advance();
        if (upper2 == "OFF") {
            enable = false;
        } else {
            throw ParseError("Expected ON or OFF after PRINTZONES");
        }
    }
    return std::make_unique<OptionPrintZonesStmt>(enable);
}

} // namespace gwbasic
