// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_RTTI_H
#define BASIC_COMPILER_AST_RTTI_H

#include "basic_compiler/ast/Node.h"

namespace gwbasic {

// LLVM-style isa/dyn_cast support using NodeKind and classof() on types.

template <typename T>
bool isa(const Node* node_ptr) {
    return T::classof(node_ptr);
}

template <typename T>
T* dyn_cast(Node* node_ptr) {
    return T::classof(node_ptr) ? static_cast<T*>(node_ptr) : nullptr;
}

template <typename T>
const T* dyn_cast(const Node* node_ptr) {
    return T::classof(node_ptr) ? static_cast<const T*>(node_ptr) : nullptr;
}

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_RTTI_H
