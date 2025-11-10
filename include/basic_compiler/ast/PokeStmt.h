// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_POKESTMT_H
#define BASIC_COMPILER_AST_POKESTMT_H

#include <memory>

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
    PokeStmt(std::unique_ptr<Expr> a, std::unique_ptr<Expr> v) : ASTLeaf(), address(std::move(a)), value(std::move(v)) {}
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_POKESTMT_H
