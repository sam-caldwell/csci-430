// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/DefSegStmt.h"
#include "basic_compiler/ast/make_node.h"

namespace gwbasic {

/*
 * Function: Parser::parseDefSeg
 * Summary:
 *  Parse DEF SEG [= expr] into a DefSegStmt.
 * Parameters:
 *  - none
 * Returns:
 *  - std::unique_ptr<Stmt>: DefSegStmt with optional segment expression
 */
std::unique_ptr<Stmt> Parser::parseDefSeg() {
    // We have already consumed DEF and SEG before calling here.
    std::unique_ptr<Expr> v;
    if (match(TokenType::Assign)) {
        v = parseExpression();
    }
    return make_node<DefSegStmt>({0,0}, std::move(v));
}

} // namespace gwbasic
