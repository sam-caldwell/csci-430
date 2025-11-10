// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_POKESTMT_H
#define BASIC_COMPILER_AST_POKESTMT_H

#include <memory>
#include <utility>

#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

/**
 * Type: PokeStmt
 * Purpose:
 *  - POKE address, value — write a byte into memory (unsafe).
 * Inputs:
 *  - address: Numeric address expression
 *  - value: Numeric byte value expression
 * Outputs:
 *  - Concrete Stmt node; semantics validate numeric types and log unsafe usage
 */
struct PokeStmt final : ASTLeaf<NodeKind::PokeStmt, Stmt> {
    std::unique_ptr<Expr> address;
    std::unique_ptr<Expr> value;
    PokeStmt(std::unique_ptr<Expr> address_expr, std::unique_ptr<Expr> value_expr)
        : address(std::move(address_expr)), value(std::move(value_expr)) {}
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_POKESTMT_H
