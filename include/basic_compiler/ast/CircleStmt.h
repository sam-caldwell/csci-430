// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <memory>
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeTemplate.h"

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
    CircleStmt(std::unique_ptr<Expr> x_, std::unique_ptr<Expr> y_, std::unique_ptr<Expr> r_,
               std::unique_ptr<Expr> c_, std::unique_ptr<Expr> s_, std::unique_ptr<Expr> e_,
               std::unique_ptr<Expr> a_, bool st)
        : ASTLeaf(), x(std::move(x_)), y(std::move(y_)), r(std::move(r_)),
          color(std::move(c_)), start(std::move(s_)), end(std::move(e_)), aspect(std::move(a_)), step(st) {}
};

} // namespace gwbasic
