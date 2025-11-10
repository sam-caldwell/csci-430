// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_SCREENSTMT_H
#define BASIC_COMPILER_AST_SCREENSTMT_H

#include <memory>

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
    ScreenStmt(std::unique_ptr<Expr> m,
               std::unique_ptr<Expr> cs,
               std::unique_ptr<Expr> ap,
               std::unique_ptr<Expr> vp)
        : ASTLeaf(), mode(std::move(m)), colorSwitch(std::move(cs)), aPage(std::move(ap)), vPage(std::move(vp)) {}
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_SCREENSTMT_H
