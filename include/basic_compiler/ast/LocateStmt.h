// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_LOCATESTMT_H
#define BASIC_COMPILER_AST_LOCATESTMT_H

#include <memory>
#include <utility>

#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/Node.h"
#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/SourcePos.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

// LOCATE row[, col]
struct LocateStmt : ASTLeaf<NodeKind::LocateStmt, Stmt> {
    std::unique_ptr<Expr> row;
    std::unique_ptr<Expr> col; // optional
    LocateStmt() = default;
    LocateStmt(const SourcePos& pos, std::unique_ptr<Expr> row_expr, std::unique_ptr<Expr> col_expr)
        : ASTLeaf<NodeKind::LocateStmt, Stmt>(pos), row(std::move(row_expr)), col(std::move(col_expr)) {}
    static bool classof(const Node* node_ptr) {
        return node_ptr != nullptr && node_ptr->kind == NodeKind::LocateStmt;
    }
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_LOCATESTMT_H
