// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_DEFFNSTMT_H
#define BASIC_COMPILER_AST_DEFFNSTMT_H

#include <memory>
#include <string>
#include <utility>

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

    DefFnStmt(std::string function_name,
              std::string parameter_name,
              std::unique_ptr<Expr> body_expr)
        : fnName(std::move(function_name)),
          paramName(std::move(parameter_name)),
          body(std::move(body_expr)) {}
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_DEFFNSTMT_H
