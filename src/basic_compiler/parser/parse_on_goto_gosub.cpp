// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/OnGosubStmt.h"
#include "basic_compiler/ast/OnGotoStmt.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/parser/ParseError.h"
#include "basic_compiler/token/TokenType.h"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace gwbasic {

/*
 * Function: Parser::parseOnGotoGosub
 * Inputs:
 *  - none (assumes KwOn already consumed)
 * Outputs:
 *  - OnGotoStmt or OnGosubStmt depending on keyword following expression
 * Theory of operation:
 *  - Parses: ON <expr> GOTO l1[,l2[,...]] | ON <expr> GOSUB l1[,l2[,...]]
 */
std::unique_ptr<Stmt> Parser::parseOnGotoGosub() {
    // Parse index expression
    auto idx = parseExpression();
    // Expect GOTO or GOSUB
    bool gosub = false;
    if (match(TokenType::KwGoto)) {
        gosub = false;
    } else if (match(TokenType::KwGosub)) {
        gosub = true;
    } else {
        throw ParseError("Expected GOTO or GOSUB after ON <expr>");
    }
    // Parse at least one integer line target, comma-separated
    std::vector<int> targets;
    if (!check(TokenType::Integer)) {
        throw ParseError("Expected line number after GOTO/GOSUB");
    }
    // First item
    {
        if (!check(TokenType::Integer)) {
            throw ParseError("Expected line number in ON dispatch list");
        }
        const int lineNumber = std::stoi(peek().lexeme);
        advance();
        targets.push_back(lineNumber);
    }
    // Remaining comma-separated items
    while (match(TokenType::Comma)) {
        if (!check(TokenType::Integer)) {
            throw ParseError("Expected line number in ON dispatch list");
        }
        const int lineNumber = std::stoi(peek().lexeme);
        advance();
        targets.push_back(lineNumber);
    }

    if (gosub) {
        return make_node<OnGosubStmt>({0,0}, std::move(idx), std::move(targets));
    }
    return make_node<OnGotoStmt>({0,0}, std::move(idx), std::move(targets));
}

} // namespace gwbasic
