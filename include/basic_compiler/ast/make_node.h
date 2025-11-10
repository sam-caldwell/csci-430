// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_MAKE_NODE_H
#define BASIC_COMPILER_AST_MAKE_NODE_H

#include <memory>
#include <utility>
#include "basic_compiler/ast/SourcePos.h"

namespace gwbasic {

// Helper to construct a node and attach a SourcePos in one call.
template <typename T, typename... Args>
inline std::unique_ptr<T> make_node(const SourcePos& pos, Args&&... args) {
    auto n = std::make_unique<T>(std::forward<Args>(args)...);
    n->pos = pos;
    return n;
}

} // namespace gwbasic
 
#endif // BASIC_COMPILER_AST_MAKE_NODE_H
