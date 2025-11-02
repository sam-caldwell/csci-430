// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <memory>
#include <string>
#include <vector>
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Traits.h"

namespace gwbasic {

/**
 * Type: ForStmt
 * Purpose:
 *  - Inline FOR loop with optional STEP and a single-line body terminating
 *    at the matching NEXT.
 * Inputs:
 *  - var: Induction variable name
 *  - start: Initial value expression
 *  - end: Terminal bound (inclusive)
 *  - step: Optional step (defaults to 1.0 when null)
 *  - body: Owned statements executed each iteration
 * Outputs:
 *  - Concrete Stmt node; codegen emits PHI-like loop with compare/inc
 * Theory of operation:
 *  - Generator lowers to labeled blocks with loop cond/body/inc structure.
 */
struct ForStmt : ASTLeaf<NodeKind::ForStmt, Stmt, ForStmtTraits> {
    std::string var;
    std::unique_ptr<Expr> start;
    std::unique_ptr<Expr> end;
    std::unique_ptr<Expr> step; // may be null -> default 1
    std::vector<std::unique_ptr<Stmt>> body; // inline for body until NEXT (same line)
    bool inlineNext{false}; // true if NEXT consumed on same line
    ForStmt(std::string v, std::unique_ptr<Expr> s, std::unique_ptr<Expr> e, std::unique_ptr<Expr> st)
        : ASTLeaf(), var(std::move(v)), start(std::move(s)), end(std::move(e)), step(std::move(st)) {}
};

} // namespace gwbasic
