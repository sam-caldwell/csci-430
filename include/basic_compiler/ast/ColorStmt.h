// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <memory>
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

struct ColorStmt final : ASTLeaf<NodeKind::ColorStmt, Stmt> {
    std::unique_ptr<Expr> fg;
    std::unique_ptr<Expr> bg;
    std::unique_ptr<Expr> border;
    ColorStmt(std::unique_ptr<Expr> f, std::unique_ptr<Expr> b, std::unique_ptr<Expr> br)
        : ASTLeaf(), fg(std::move(f)), bg(std::move(b)), border(std::move(br)) {}
};

} // namespace gwbasic

