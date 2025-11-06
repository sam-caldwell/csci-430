// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <memory>
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeTemplate.h"

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
    explicit CallAbsStmt(std::unique_ptr<Expr> a) : ASTLeaf(), address(std::move(a)) {}
};

} // namespace gwbasic
