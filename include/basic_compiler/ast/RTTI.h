// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_RTTI_H
#define BASIC_COMPILER_AST_RTTI_H

#include "basic_compiler/ast/Node.h"

namespace gwbasic {

// LLVM-style isa/dyn_cast support using NodeKind and classof() on types.

template <typename T>
bool isa(const Node* N) {
    return T::classof(N);
}

template <typename T>
T* dyn_cast(Node* N) {
    return T::classof(N) ? static_cast<T*>(N) : nullptr;
}

template <typename T>
const T* dyn_cast(const Node* N) {
    return T::classof(N) ? static_cast<const T*>(N) : nullptr;
}

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_RTTI_H
