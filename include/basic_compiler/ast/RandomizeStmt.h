// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_RANDOMIZESTMT_H
#define BASIC_COMPILER_AST_RANDOMIZESTMT_H

#include <memory>
#include <utility>

#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

/**
 * Type: RandomizeStmt
 * Purpose:
 *  - RANDOMIZE [expr] to seed the RNG. If expr missing, seed from system time.
 */
struct RandomizeStmt : ASTLeaf<NodeKind::RandomizeStmt, Stmt> {
    std::unique_ptr<Expr> seed; // optional
    RandomizeStmt() = default;
    explicit RandomizeStmt(std::unique_ptr<Expr> seed_expr)
        : seed(std::move(seed_expr)) {}
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_RANDOMIZESTMT_H
