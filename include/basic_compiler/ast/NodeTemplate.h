// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_NODETEMPLATE_H
#define BASIC_COMPILER_AST_NODETEMPLATE_H

#include "basic_compiler/ast/Node.h"
#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/SourcePos.h"

namespace gwbasic {

/**
 * Template: ASTLeaf
 * Purpose:
 *  - DRY helper to implement common boilerplate for concrete AST nodes.
 *  - Provides constructor wiring NodeKind to the base and a static classof.
 * Usage:
 *  - struct NumberExpr : ASTLeaf<NodeKind::NumberExpr, Expr> { ... };
 */
/** Default traits for AST leaves (placeholder for future customization). */
struct DefaultNodeTraits { };

template <NodeKind K, typename Base, typename Traits = DefaultNodeTraits>
struct ASTLeaf : Base {
    using TraitsT = Traits;
    ASTLeaf() : Base(K) {}
    explicit ASTLeaf(const SourcePos& pos) : Base(K) { this->pos = pos; }
    static bool classof(const Node* node_ptr) {
        return node_ptr != nullptr && node_ptr->kind == K;
    }
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_NODETEMPLATE_H
