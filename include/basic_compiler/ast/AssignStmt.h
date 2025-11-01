// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <memory>
#include <string>
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

/**
 * Type: AssignStmt
 * Purpose:
 *  - LET or implicit assignment of an expression to a variable.
 * Inputs:
 *  - name: Variable identifier
 *  - value: Expression to evaluate and store
 * Outputs:
 *  - Concrete Stmt node; codegen ensures allocation and store to the symbol
 * Theory of operation:
 *  - Codegen emits store to an alloca location tracked per variable name.
 */
struct AssignStmt : ASTLeaf<NodeKind::AssignStmt, Stmt> {
    std::string name;
    std::unique_ptr<Expr> value;
    AssignStmt(std::string n, std::unique_ptr<Expr> v)
        : ASTLeaf(), name(std::move(n)), value(std::move(v)) {}
};

} // namespace gwbasic
