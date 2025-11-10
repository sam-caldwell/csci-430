// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_KILLSTMT_H
#define BASIC_COMPILER_AST_KILLSTMT_H

#include <memory>
#include <utility>

#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

/** KillStmt: KILL filespec$ */
struct KillStmt final : ASTLeaf<NodeKind::KillStmt, Stmt> {
    std::unique_ptr<Expr> filespec;
    explicit KillStmt(std::unique_ptr<Expr> filespec_expr)
        : filespec(std::move(filespec_expr)) {}
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_KILLSTMT_H
