// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <memory>
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

struct PokeStmt : ASTLeaf<NodeKind::PokeStmt, Stmt> {
    std::unique_ptr<Expr> address;
    std::unique_ptr<Expr> value;
    PokeStmt(std::unique_ptr<Expr> a, std::unique_ptr<Expr> v) : ASTLeaf(), address(std::move(a)), value(std::move(v)) {}
};

} // namespace gwbasic

