// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_BLOADSTMT_H
#define BASIC_COMPILER_AST_BLOADSTMT_H

#include <memory>
#include <utility>

#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

/**
 * Type: BloadStmt
 * Purpose:
 *  - BLOAD filename[, offset] — load binary file into memory (unsafe; logged).
 * Inputs:
 *  - filename: String expression naming the file
 *  - offset: Optional numeric address expression
 * Outputs:
 *  - Concrete Stmt node; semantics validate types and log unsafe usage
 */
struct BloadStmt final : ASTLeaf<NodeKind::BloadStmt, Stmt> {
    std::unique_ptr<Expr> filename; // string expr
    std::unique_ptr<Expr> offset;   // optional numeric
    BloadStmt(std::unique_ptr<Expr> filename_expr, std::unique_ptr<Expr> offset_expr)
        : filename(std::move(filename_expr)), offset(std::move(offset_expr)) {}
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_BLOADSTMT_H
