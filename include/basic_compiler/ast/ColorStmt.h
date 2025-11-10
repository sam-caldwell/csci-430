// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_COLORSTMT_H
#define BASIC_COMPILER_AST_COLORSTMT_H

#include <memory>
#include <utility>

#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

/**
 * Type: ColorStmt
 * Purpose:
 *  - COLOR fg[, bg[, border]] — set text colors (emitted via ANSI SGR).
 * Inputs:
 *  - fg: Foreground color numeric expression (optional)
 *  - bg: Background color numeric expression (optional)
 *  - border: Border color numeric expression (optional)
 * Outputs:
 *  - Concrete Stmt node; codegen emits printf with SGR sequences
 */
struct ColorStmt final : ASTLeaf<NodeKind::ColorStmt, Stmt> {
    std::unique_ptr<Expr> fg;
    std::unique_ptr<Expr> bg;
    std::unique_ptr<Expr> border;
    ColorStmt(std::unique_ptr<Expr> foreground_expr,
              std::unique_ptr<Expr> background_expr,
              std::unique_ptr<Expr> border_expr)
        : fg(std::move(foreground_expr)),
          bg(std::move(background_expr)),
          border(std::move(border_expr)) {}
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_COLORSTMT_H
