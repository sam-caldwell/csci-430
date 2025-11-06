// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <string>
#include <vector>
#include <memory>
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/ReadTarget.h"

namespace gwbasic {

/**
 * Type: SwapStmt
 * Purpose:
 *  - SWAP x, y — exchange the contents of two variables or array elements.
 * Inputs:
 *  - left/right: variable references (scalar or array element indices)
 */
struct SwapStmt final : ASTLeaf<NodeKind::SwapStmt, Stmt> {
    ReadTarget left;
    ReadTarget right;
    SwapStmt() = default;
    SwapStmt(ReadTarget l, ReadTarget r) : ASTLeaf(), left(std::move(l)), right(std::move(r)) {}
};

} // namespace gwbasic

