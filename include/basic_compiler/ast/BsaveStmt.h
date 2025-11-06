// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <memory>
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

/**
 * Type: BsaveStmt
 * Purpose:
 *  - BSAVE filename, offset, length — save memory to a binary file (unsafe).
 * Inputs:
 *  - filename: String expression naming the file
 *  - offset: Numeric start address expression
 *  - length: Numeric byte-count expression
 * Outputs:
 *  - Concrete Stmt node; semantics validate types and log unsafe usage
 */
struct BsaveStmt final : ASTLeaf<NodeKind::BsaveStmt, Stmt> {
    std::unique_ptr<Expr> filename; // string expr
    std::unique_ptr<Expr> offset;   // numeric
    std::unique_ptr<Expr> length;   // numeric
    BsaveStmt(std::unique_ptr<Expr> fn, std::unique_ptr<Expr> off, std::unique_ptr<Expr> len)
        : ASTLeaf(), filename(std::move(fn)), offset(std::move(off)), length(std::move(len)) {}
};

} // namespace gwbasic
