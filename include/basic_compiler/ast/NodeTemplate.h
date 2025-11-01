// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include "basic_compiler/ast/Node.h"

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
    explicit ASTLeaf(const SourcePos& p) : Base(K) { this->pos = p; }
    static bool classof(const Node* N) { return N && N->kind == K; }
};

} // namespace gwbasic
