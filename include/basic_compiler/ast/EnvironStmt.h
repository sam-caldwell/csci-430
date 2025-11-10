// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_ENVIRONSTMT_H
#define BASIC_COMPILER_AST_ENVIRONSTMT_H

#include <memory>
#include <utility>

#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

/** EnvironStmt: ENVIRON stringexpr ("NAME=VALUE") */
struct EnvironStmt final : ASTLeaf<NodeKind::EnvironStmt, Stmt> {
    std::unique_ptr<Expr> spec;
    explicit EnvironStmt(std::unique_ptr<Expr> spec_expr)
        : spec(std::move(spec_expr)) {}
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_ENVIRONSTMT_H
