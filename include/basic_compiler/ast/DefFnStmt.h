// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_DEFFNSTMT_H
#define BASIC_COMPILER_AST_DEFFNSTMT_H

#include <memory>
#include <string>

#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

/**
 * Type: DefFnStmt
 * Purpose:
 *  - GW-BASIC user-defined single-expression function definition.
 * Syntax:
 *  - DEF FNname(param) = expression
 * Notes:
 *  - Return type is inferred from function name suffix ('$' => string).
 *  - Parameter type inferred by its name suffix ('$' => string).
 */
struct DefFnStmt : ASTLeaf<NodeKind::DefFnStmt, Stmt> {
    std::string fnName;     // e.g., "FNSQ" or "FNS$" (includes trailing '$' if present)
    std::string paramName;  // e.g., "X" or "S$"
    std::unique_ptr<Expr> body; // expression defining the function

    DefFnStmt(std::string fn, std::string pn, std::unique_ptr<Expr> b)
        : ASTLeaf(), fnName(std::move(fn)), paramName(std::move(pn)), body(std::move(b)) {}
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_DEFFNSTMT_H
