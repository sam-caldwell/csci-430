// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <memory>
#include <optional>
#include <string>
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

// DEF USR[digits] = expr (address)
struct DefUsrStmt : ASTLeaf<NodeKind::DefUsrStmt, Stmt> {
    std::optional<int> index; // optional USR index (e.g., USR0..USR9); not used semantically here
    std::unique_ptr<Expr> address;
    DefUsrStmt(std::optional<int> i, std::unique_ptr<Expr> a)
        : ASTLeaf(), index(i), address(std::move(a)) {}
};

} // namespace gwbasic

