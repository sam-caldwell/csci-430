// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <memory>
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

struct BloadStmt final : ASTLeaf<NodeKind::BloadStmt, Stmt> {
    std::unique_ptr<Expr> filename; // string expr
    std::unique_ptr<Expr> offset;   // optional numeric
    BloadStmt(std::unique_ptr<Expr> fn, std::unique_ptr<Expr> off)
        : ASTLeaf(), filename(std::move(fn)), offset(std::move(off)) {}
};

} // namespace gwbasic

