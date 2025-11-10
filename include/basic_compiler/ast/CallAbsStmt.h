// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_CALLABSSTMT_H
#define BASIC_COMPILER_AST_CALLABSSTMT_H

#include <memory>
#include <utility>

#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

/**
 * Type: CallAbsStmt
 * Purpose:
 *  - CALL absolute-address routine (unsafe call into memory).
 * Inputs:
 *  - address: Numeric expression providing the offset within current segment
 * Outputs:
 *  - Concrete Stmt node; semantics validate numeric type and log unsafe usage
 */
struct CallAbsStmt final : ASTLeaf<NodeKind::CallAbsStmt, Stmt> {
    std::unique_ptr<Expr> address;
    explicit CallAbsStmt(std::unique_ptr<Expr> address_expr)
        : address(std::move(address_expr)) {}
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_CALLABSSTMT_H
