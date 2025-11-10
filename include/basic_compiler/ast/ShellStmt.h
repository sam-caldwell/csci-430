// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_SHELLSTMT_H
#define BASIC_COMPILER_AST_SHELLSTMT_H

#include <memory>
#include <utility>

#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

/** ShellStmt: SHELL [command$] */
struct ShellStmt final : ASTLeaf<NodeKind::ShellStmt, Stmt> {
    std::unique_ptr<Expr> command; // optional
    ShellStmt() = default;
    explicit ShellStmt(std::unique_ptr<Expr> command_expr)
        : command(std::move(command_expr)) {}
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_SHELLSTMT_H
