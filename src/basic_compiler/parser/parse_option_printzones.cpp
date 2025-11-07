// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/OptionPrintZonesStmt.h"

namespace gwbasic {

/*
 * Function: Parser::parseOptionPrintZones
 * Inputs:
 *  - none (assumes OPTION already consumed and next is PRINTZONES)
 * Outputs:
 *  - OptionPrintZonesStmt with enabled set by ON/OFF
 * Theory of operation:
 *  - Accepts: OPTION PRINTZONES ON | OPTION PRINTZONES OFF
 */
std::unique_ptr<Stmt> Parser::parseOptionPrintZones() {
    // Expect identifier PRINTZONES
    if (peek().type != TokenType::Identifier) throw ParseError("Expected PRINTZONES after OPTION");
    std::string up = peek().lexeme; for (auto &ch : up) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch))); advance();
    if (up != "PRINTZONES") throw ParseError("Expected PRINTZONES after OPTION");
    // Expect ON or OFF
    bool enable = false;
    if (match(TokenType::KwOn)) {
        enable = true;
    } else {
        // OFF may be lexed as Identifier
        if (peek().type != TokenType::Identifier) throw ParseError("Expected ON or OFF after PRINTZONES");
        std::string u2 = peek().lexeme; for (auto &ch : u2) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch))); advance();
        if (u2 == "OFF") enable = false; else throw ParseError("Expected ON or OFF after PRINTZONES");
    }
    return std::make_unique<OptionPrintZonesStmt>(enable);
}

} // namespace gwbasic

