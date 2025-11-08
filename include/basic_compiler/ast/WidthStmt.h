// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <memory>
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

/**
 * Type: WidthStmt
 * Purpose:
 *  - WIDTH [device$,] columns — configure output width or device width.
 * Notes:
 *  - For now, this compiler treats WIDTH as a no-op at runtime. Semantics
 *    performs basic type checks and logs the occurrence for visibility.
 */
struct WidthStmt final : ASTLeaf<NodeKind::WidthStmt, Stmt> {
    std::unique_ptr<Expr> device;   // optional string expression (e.g., "LPT1:")
    std::unique_ptr<Expr> columns;  // numeric expression indicating width
    WidthStmt() = default;
    WidthStmt(std::unique_ptr<Expr> dev, std::unique_ptr<Expr> cols)
        : ASTLeaf<NodeKind::WidthStmt, Stmt>(), device(std::move(dev)), columns(std::move(cols)) {}
    static bool classof(const Node* N) { return N && N->kind == NodeKind::WidthStmt; }
};

} // namespace gwbasic
