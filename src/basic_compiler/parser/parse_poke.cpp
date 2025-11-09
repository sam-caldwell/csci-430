// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/PokeStmt.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/token/TokenType.h"
#include <memory>
#include <utility>

namespace gwbasic {

/*
 * Function: Parser::parsePoke
 * Purpose:
 *  - Parse POKE address, value
 * Inputs:
 *  - none (assumes 'POKE' matched by caller)
 * Outputs:
 *  - PokeStmt: address and value expressions
 */
std::unique_ptr<Stmt> Parser::parsePoke() {
    auto addr = parseExpression();
    consume(TokenType::Comma, ",");
    auto val = parseExpression();
    return make_node<PokeStmt>({0,0}, std::move(addr), std::move(val));
}

} // namespace gwbasic
