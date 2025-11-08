// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

// CLS: Clear the virtual screen and reset cursor
struct ClsStmt : ASTLeaf<NodeKind::ClsStmt, Stmt> {
    ClsStmt() = default;
    explicit ClsStmt(const SourcePos& p) : ASTLeaf<NodeKind::ClsStmt, Stmt>(p) {}
    static bool classof(const Node* N) { return N && N->kind == NodeKind::ClsStmt; }
};

} // namespace gwbasic

