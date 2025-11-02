// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <memory>
#include <vector>
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

/**
 * Type: IfBlockStmt
 * Purpose: Multi-line IF ... THEN [ ... ELSE ... ] END IF block.
 * Inputs:
 *  - cond: Boolean comparison expression
 *  - thenBody: statements when cond true
 *  - elseBody: optional statements when cond false
 */
struct IfBlockStmt : ASTLeaf<NodeKind::IfBlockStmt, Stmt> {
    std::unique_ptr<Expr> cond;
    std::vector<std::unique_ptr<Stmt>> thenBody;
    std::vector<std::unique_ptr<Stmt>> elseBody; // empty if no ELSE
    explicit IfBlockStmt(std::unique_ptr<Expr> c) : ASTLeaf(), cond(std::move(c)) {}
};

/** Marker: ElseStmt used by the parser prior to restructuring. */
struct ElseStmt : ASTLeaf<NodeKind::ElseStmt, Stmt> {
    ElseStmt() = default;
};

/** Marker: EndIfStmt used by the parser prior to restructuring. */
struct EndIfStmt : ASTLeaf<NodeKind::EndIfStmt, Stmt> {
    EndIfStmt() = default;
};

} // namespace gwbasic

