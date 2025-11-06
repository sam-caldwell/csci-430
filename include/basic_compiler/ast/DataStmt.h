// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <string>
#include <vector>
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/DataItem.h"

namespace gwbasic {

/**
 * Type: DataStmt
 * Purpose:
 *  - Holds compile-time DATA list items preserving quoted-string vs numeric
 *    tokenization and their normalized text payload in source order.
 */
struct DataStmt : ASTLeaf<NodeKind::DataStmt, Stmt> {
    std::vector<DataItem> items;
    DataStmt() = default;
    explicit DataStmt(std::vector<DataItem> v) : ASTLeaf(), items(std::move(v)) {}
};

} // namespace gwbasic
