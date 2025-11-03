// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include <string>

namespace gwbasic {

/**
 * Type: ArrayAssignStmt
 * Purpose:
 *  - Assignment to an array element: A(i) = expr (1-D only).
 */
struct ArrayAssignStmt : ASTLeaf<NodeKind::ArrayAssignStmt, Stmt> {
    std::string name;
    std::unique_ptr<Expr> index;
    std::unique_ptr<Expr> value;
    ArrayAssignStmt() = default;
    ArrayAssignStmt(std::string n, std::unique_ptr<Expr> idx, std::unique_ptr<Expr> v)
        : ASTLeaf(), name(std::move(n)), index(std::move(idx)), value(std::move(v)) {}
};

} // namespace gwbasic
