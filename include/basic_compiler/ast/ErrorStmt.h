// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_ERRORSTMT_H
#define BASIC_COMPILER_AST_ERRORSTMT_H

#include <memory>
#include <utility>

#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

/**
 * Type: ErrorStmt
 * Purpose:
 *  - Raise a runtime error with a numeric error code.
 * Inputs:
 *  - code: numeric expression evaluated to an integer error number.
 */
struct ErrorStmt : ASTLeaf<NodeKind::ErrorStmt, Stmt> {
    std::unique_ptr<Expr> code;
    explicit ErrorStmt(std::unique_ptr<Expr> code_expr)
        : code(std::move(code_expr)) {}
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_ERRORSTMT_H
