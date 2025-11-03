// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <memory>
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

struct CallAbsStmt : ASTLeaf<NodeKind::CallAbsStmt, Stmt> {
    std::unique_ptr<Expr> address;
    explicit CallAbsStmt(std::unique_ptr<Expr> a) : ASTLeaf(), address(std::move(a)) {}
};

} // namespace gwbasic

