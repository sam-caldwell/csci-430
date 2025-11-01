#pragma once

#include <memory>
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

