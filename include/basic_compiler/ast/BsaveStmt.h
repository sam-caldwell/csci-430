// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_BSAVESTMT_H
#define BASIC_COMPILER_AST_BSAVESTMT_H

#include <memory>
#include <utility>

#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

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
    BsaveStmt(std::unique_ptr<Expr> filename_expr,
              std::unique_ptr<Expr> offset_expr,
              std::unique_ptr<Expr> length_expr)
        : filename(std::move(filename_expr)),
          offset(std::move(offset_expr)),
          length(std::move(length_expr)) {}
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_BSAVESTMT_H
