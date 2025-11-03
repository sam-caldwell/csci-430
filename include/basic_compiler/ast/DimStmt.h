// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

/**
 * Type: DimStmt
 * Purpose:
 *  - Declare one 1-D array with a fixed length: DIM A(10)
 */
struct DimStmt : ASTLeaf<NodeKind::DimStmt, Stmt> {
    std::string name;
    int length{}; // number of elements (0-based indexing assumed)
    DimStmt() = default;
    DimStmt(std::string n, int len) : ASTLeaf(), name(std::move(n)), length(len) {}
};

} // namespace gwbasic
