// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <memory>
#include <vector>
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

/**
 * Type: PrintStmt
 * Purpose:
 *  - PRINT a string or numeric expression using printf.
 * Inputs:
 *  - value: Expression to print (StringExpr or numeric Expr)
 * Outputs:
 *  - Concrete Stmt node; codegen routes to @printf with proper format
 * Theory of operation:
 *  - String literals use "%s\n"; numeric expressions use "%f\n".
 */
struct PrintStmt : ASTLeaf<NodeKind::PrintStmt, Stmt> {
    // Backward-compatible single value; additional items in 'more'
    std::unique_ptr<Expr> value;
    std::vector<std::unique_ptr<Expr>> more;

    explicit PrintStmt(std::unique_ptr<Expr> v) : ASTLeaf(), value(std::move(v)) {}
    explicit PrintStmt(std::vector<std::unique_ptr<Expr>> v) : ASTLeaf() {
        if (!v.empty()) {
            value = std::move(v.front());
            more.reserve(v.size() - 1);
            for (size_t i = 1; i < v.size(); ++i) more.emplace_back(std::move(v[i]));
        }
    }
};

} // namespace gwbasic
