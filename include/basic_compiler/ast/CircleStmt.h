// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_CIRCLESTMT_H
#define BASIC_COMPILER_AST_CIRCLESTMT_H

#include <memory>
#include <utility>

#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

/**
 * Type: CircleStmt
 * Purpose:
 *  - CIRCLE x, y, r — draw a circle at (x,y) with radius r.
 * Notes:
 *  - Stubbed implementation guarded by a runtime graphics-ready flag.
 */
struct CircleStmt final : ASTLeaf<NodeKind::CircleStmt, Stmt> {
    std::unique_ptr<Expr> x;
    std::unique_ptr<Expr> y;
    std::unique_ptr<Expr> r;
    std::unique_ptr<Expr> color;   // optional
    std::unique_ptr<Expr> start;   // optional
    std::unique_ptr<Expr> end;     // optional
    std::unique_ptr<Expr> aspect;  // optional
    bool step{false};              // optional STEP for relative coords (parsed but not used now)
    // NOLINTNEXTLINE(readability-function-size)
    CircleStmt(std::unique_ptr<Expr> x_expr,
               std::unique_ptr<Expr> y_expr,
               std::unique_ptr<Expr> radius_expr,
               std::unique_ptr<Expr> color_expr,
               std::unique_ptr<Expr> start_expr,
               std::unique_ptr<Expr> end_expr,
               std::unique_ptr<Expr> aspect_expr,
               bool is_step)
        : x(std::move(x_expr)),
          y(std::move(y_expr)),
          r(std::move(radius_expr)),
          color(std::move(color_expr)),
          start(std::move(start_expr)),
          end(std::move(end_expr)),
          aspect(std::move(aspect_expr)),
          step(is_step) {}
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_CIRCLESTMT_H
