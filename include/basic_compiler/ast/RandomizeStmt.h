// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <memory>
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

/**
 * Type: RandomizeStmt
 * Purpose:
 *  - RANDOMIZE [expr] to seed the RNG. If expr missing, seed from system time.
 */
struct RandomizeStmt : ASTLeaf<NodeKind::RandomizeStmt, Stmt> {
    std::unique_ptr<Expr> seed; // optional
    RandomizeStmt() : ASTLeaf() {}
    explicit RandomizeStmt(std::unique_ptr<Expr> s) : ASTLeaf(), seed(std::move(s)) {}
};

} // namespace gwbasic
