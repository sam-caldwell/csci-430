// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_LOCATESTMT_H
#define BASIC_COMPILER_AST_LOCATESTMT_H

#include <memory>

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
    LocateStmt(const SourcePos& p, std::unique_ptr<Expr> r, std::unique_ptr<Expr> c)
        : ASTLeaf<NodeKind::LocateStmt, Stmt>(p), row(std::move(r)), col(std::move(c)) {}
    static bool classof(const Node* N) { return N && N->kind == NodeKind::LocateStmt; }
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_LOCATESTMT_H
