// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/NameStmt.h"
#include "basic_compiler/ast/make_node.h"

namespace gwbasic {

/*
 * Function: Parser::parseName
 * Summary:
 *  Parse NAME old AS new statement for renaming a file.
 * Parameters:
 *  - none
 * Returns:
 *  - std::unique_ptr<Stmt>: NameStmt with old/new expressions
 */
std::unique_ptr<Stmt> Parser::parseName() {
    auto old = parseExpression();
    consume(TokenType::KwAs, "AS");
    auto neu = parseExpression();
    return make_node<NameStmt>({0,0}, std::move(old), std::move(neu));
}

} // namespace gwbasic
