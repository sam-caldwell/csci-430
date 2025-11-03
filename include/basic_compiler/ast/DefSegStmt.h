// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <memory>
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

// DEF SEG [= expr] — sets default memory segment for PEEK/POKE/etc. (no-op here)
struct DefSegStmt : ASTLeaf<NodeKind::DefSegStmt, Stmt> {
    std::unique_ptr<Expr> value; // null means restore default
    explicit DefSegStmt(std::unique_ptr<Expr> v) : ASTLeaf(), value(std::move(v)) {}
};

} // namespace gwbasic

