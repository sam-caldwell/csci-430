// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include "basic_compiler/ast/SourcePos.h"
#include "basic_compiler/ast/NodeKind.h"

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
    explicit Node(NodeKind k) : kind(k) {}
public:
    NodeKind getKind() const { return kind; }
};

} // namespace gwbasic

