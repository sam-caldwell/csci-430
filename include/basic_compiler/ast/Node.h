// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_NODE_H
#define BASIC_COMPILER_AST_NODE_H

#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/SourcePos.h"

namespace gwbasic {

/**
 * Base: Node
 *  - Common base for all AST nodes providing kind and source position.
 */
struct Node {
    virtual ~Node() = default;
    NodeKind kind;
    SourcePos pos{};
protected:
    explicit Node(const NodeKind k) : kind(k) {}
public:
    [[nodiscard]] NodeKind getKind() const { return kind; }
    /** Fluent setter to unify position assignment. */
    Node& setPos(const SourcePos& p) { pos = p; return *this; }
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_NODE_H
