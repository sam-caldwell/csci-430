// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include <string>
#include <vector>

namespace gwbasic {

/**
 * Type: ArrayAssignStmt
 * Purpose:
 *  - Assignment to an array element: A(i[,j...]) = expr (N-D supported).
 */
struct ArrayAssignStmt : ASTLeaf<NodeKind::ArrayAssignStmt, Stmt> {
    std::string name;
    std::vector<std::unique_ptr<Expr>> indices;
    std::unique_ptr<Expr> value;
    ArrayAssignStmt() = default;
    ArrayAssignStmt(std::string n, std::vector<std::unique_ptr<Expr>> idx, std::unique_ptr<Expr> v)
        : ASTLeaf(), name(std::move(n)), indices(std::move(idx)), value(std::move(v)) {}
};

} // namespace gwbasic
