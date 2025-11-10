#ifndef BASIC_COMPILER_AST_CLSSTMT_H
#define BASIC_COMPILER_AST_CLSSTMT_H
// (c) 2025 Sam Caldwell. All Rights Reserved.

#include "basic_compiler/ast/Node.h"
#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/SourcePos.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

// CLS: Clear the virtual screen and reset cursor
struct ClsStmt final : ASTLeaf<NodeKind::ClsStmt, Stmt> {
    ClsStmt() = default;
    explicit ClsStmt(const SourcePos& pos) : ASTLeaf<NodeKind::ClsStmt, Stmt>(pos) {}
    static bool classof(const Node* node_ptr) {
        return node_ptr != nullptr && node_ptr->kind == NodeKind::ClsStmt;
    }
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_CLSSTMT_H
