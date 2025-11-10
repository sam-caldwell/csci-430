// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_NAMESTMT_H
#define BASIC_COMPILER_AST_NAMESTMT_H

#include <memory>
#include <utility>

#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

/** NameStmt: NAME old$ AS new$ */
struct NameStmt final : ASTLeaf<NodeKind::NameStmt, Stmt> {
    std::unique_ptr<Expr> oldName;
    std::unique_ptr<Expr> newName;
    NameStmt(std::unique_ptr<Expr> old_name_expr, std::unique_ptr<Expr> new_name_expr)
        : oldName(std::move(old_name_expr)), newName(std::move(new_name_expr)) {}
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_NAMESTMT_H
