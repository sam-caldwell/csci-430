// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_ARRAYASSIGNSTMT_H
#define BASIC_COMPILER_AST_ARRAYASSIGNSTMT_H

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

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
    ArrayAssignStmt(std::string nameStr,
                    std::vector<std::unique_ptr<Expr>> indexExprs,
                    std::unique_ptr<Expr> valueExpr)
        : name(std::move(nameStr)), indices(std::move(indexExprs)), value(std::move(valueExpr)) {}
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_ARRAYASSIGNSTMT_H
