// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <string>
#include <vector>
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

/**
 * Type: DataStmt
 * Purpose:
 *  - Holds compile-time DATA list items as strings in source order.
 */
struct DataStmt : ASTLeaf<NodeKind::DataStmt, Stmt> {
    std::vector<std::string> items; // all values normalized to strings
    DataStmt() = default;
    explicit DataStmt(std::vector<std::string> v) : ASTLeaf(), items(std::move(v)) {}
};

} // namespace gwbasic

