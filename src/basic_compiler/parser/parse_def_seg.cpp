// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/DefSegStmt.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/token/TokenType.h"
#include <memory>
#include <utility>

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
    std::unique_ptr<Expr> valueExpr;
    if (match(TokenType::Assign)) {
        valueExpr = parseExpression();
    }
    return make_node<DefSegStmt>({0,0}, std::move(valueExpr));
}

} // namespace gwbasic
