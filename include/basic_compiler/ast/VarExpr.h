// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <string>
#include "basic_compiler/ast/Expr.h"

namespace gwbasic {

/**
 * Type: VarExpr
 * Purpose:
 *  - Reference to a scalar variable by name.
 * Inputs:
 *  - name: Identifier (case-insensitive in BASIC semantics; stored raw)
 * Outputs:
 *  - Concrete Expr node; codegen ensures allocation and loads/stores as needed
 * Theory of operation:
 *  - Codegen maintains a symbol table mapping variable names to allocas
 *    within the current function scope.
 */
struct VarExpr : Expr {
    std::string name;
    explicit VarExpr(std::string n) : Expr(NodeKind::VarExpr), name(std::move(n)) {}
    static bool classof(const Node* N) { return N && N->kind == NodeKind::VarExpr; }
};

} // namespace gwbasic
