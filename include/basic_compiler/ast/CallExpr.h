// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <memory>
#include <string>
#include <vector>
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

/**
 * Type: CallExpr
 * Purpose:
 *  - Function call expression, e.g., SQR(expr) or ABS(expr).
 * Inputs:
 *  - callee: Function name (case-insensitive in semantics/codegen)
 *  - args: Argument expressions
 */
struct CallExpr : ASTLeaf<NodeKind::CallExpr, Expr> {
    std::string callee;
    std::vector<std::unique_ptr<Expr>> args;
    explicit CallExpr(std::string name, std::vector<std::unique_ptr<Expr>> a)
        : ASTLeaf(), callee(std::move(name)), args(std::move(a)) {}
};

} // namespace gwbasic
