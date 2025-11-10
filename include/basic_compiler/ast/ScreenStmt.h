// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_SCREENSTMT_H
#define BASIC_COMPILER_AST_SCREENSTMT_H

#include <memory>
#include <utility>

#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

/**
 * Type: ScreenStmt
 * Purpose:
 *  - SCREEN [mode][,[colorswitch][,[apage][,vpage]]] — set display mode.
 * Notes:
 *  - For this compiler, text mode continues to use the terminal; any
 *    graphics mode triggers initialization of a stub graphics window.
 * Inputs:
 *  - mode: Numeric expression selecting the mode (optional; default 0)
 *  - colorswitch: Optional numeric (ignored for now)
 *  - apage/vpage: Optional numeric (ignored for now)
 * Outputs:
 *  - Concrete Stmt node; codegen may emit a call to initialize graphics.
 */
struct ScreenStmt final : ASTLeaf<NodeKind::ScreenStmt, Stmt> {
    std::unique_ptr<Expr> mode;
    std::unique_ptr<Expr> colorSwitch;
    std::unique_ptr<Expr> aPage;
    std::unique_ptr<Expr> vPage;
    ScreenStmt(std::unique_ptr<Expr> mode_expr,
               std::unique_ptr<Expr> color_switch_expr,
               std::unique_ptr<Expr> a_page_expr,
               std::unique_ptr<Expr> v_page_expr)
        : mode(std::move(mode_expr)),
          colorSwitch(std::move(color_switch_expr)),
          aPage(std::move(a_page_expr)),
          vPage(std::move(v_page_expr)) {}
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_SCREENSTMT_H
