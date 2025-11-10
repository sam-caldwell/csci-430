// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_MAKE_NODE_H
#define BASIC_COMPILER_AST_MAKE_NODE_H

#include "basic_compiler/ast/SourcePos.h"
#include <memory>
#include <utility>

namespace gwbasic {

// Helper to construct a node and attach a SourcePos in one call.
template <typename T, typename... Args>
inline std::unique_ptr<T> make_node(const SourcePos& pos, Args&&... args) {
    auto node_uptr = std::make_unique<T>(std::forward<Args>(args)...);
    node_uptr->pos = pos;
    return node_uptr;
}

} // namespace gwbasic
 
#endif // BASIC_COMPILER_AST_MAKE_NODE_H
